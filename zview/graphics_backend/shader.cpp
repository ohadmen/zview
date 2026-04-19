#include "zview/graphics_backend/shader.h"

#include "zview/graphics_backend/vulkan_context.h"

// Generated SPIR-V headers
#include <iostream>

#include "zview/graphics_backend/shader_code/edges.frag.spv.h"
#include "zview/graphics_backend/shader_code/edges.vert.spv.h"
#include "zview/graphics_backend/shader_code/grid.frag.spv.h"
#include "zview/graphics_backend/shader_code/grid.vert.spv.h"
#include "zview/graphics_backend/shader_code/mesh.frag.spv.h"
#include "zview/graphics_backend/shader_code/mesh.vert.spv.h"
#include "zview/graphics_backend/shader_code/pcl.frag.spv.h"
#include "zview/graphics_backend/shader_code/pcl.vert.spv.h"
#include "zview/graphics_backend/shader_code/picking.frag.spv.h"
#include "zview/graphics_backend/shader_code/picking.vert.spv.h"

namespace zview {

// Vertex input: binding 0, stride 16 (3 floats xyz + 4 bytes rgba packed)
static const VkVertexInputBindingDescription kBinding{
    0, 16, VK_VERTEX_INPUT_RATE_VERTEX};

static const VkVertexInputAttributeDescription kAttribs[2] = {
    {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},  // a_xyz
    {1, 0, VK_FORMAT_R8G8B8A8_UNORM, 12},   // a_rgb (normalized bytes)
};

Shader::~Shader() {
  auto& ctx = VulkanContext::get();
  if (m_pipeline != VK_NULL_HANDLE)
    vkDestroyPipeline(ctx.device, m_pipeline, nullptr);
  if (m_layout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(ctx.device, m_layout, nullptr);
}

Shader::Shader(Shader&& o) noexcept
    : m_pipeline(o.m_pipeline), m_layout(o.m_layout), m_type(o.m_type) {
  o.m_pipeline = VK_NULL_HANDLE;
  o.m_layout = VK_NULL_HANDLE;
}

Shader& Shader::operator=(Shader&& o) noexcept {
  if (this != &o) {
    auto& ctx = VulkanContext::get();
    if (m_pipeline) vkDestroyPipeline(ctx.device, m_pipeline, nullptr);
    if (m_layout) vkDestroyPipelineLayout(ctx.device, m_layout, nullptr);
    m_pipeline = o.m_pipeline;
    o.m_pipeline = VK_NULL_HANDLE;
    m_layout = o.m_layout;
    o.m_layout = VK_NULL_HANDLE;
    m_type = o.m_type;
  }
  return *this;
}

VkShaderModule Shader::createModule(const uint32_t* code, uint32_t len) const {
  VkShaderModuleCreateInfo ci{};
  ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  ci.codeSize = len * sizeof(uint32_t);
  ci.pCode = code;
  VkShaderModule mod;
  if (vkCheck(
          vkCreateShaderModule(VulkanContext::get().device, &ci, nullptr, &mod),
          "vkCreateShaderModule") != VK_SUCCESS)
    return VK_NULL_HANDLE;
  return mod;
}

bool Shader::init(ShaderType type, VkRenderPass renderPass, bool enableBlend,
                  bool enableDepth, VkPrimitiveTopology topology) {
  m_type = type;
  auto& ctx = VulkanContext::get();

  // Select SPIR-V blobs and push constant size
  const uint32_t *vertCode = nullptr, *fragCode = nullptr;
  uint32_t vertLen = 0, fragLen = 0, pcSize = 0;
  switch (type) {
    case ShaderType::PCL:
      vertCode = pcl_vert;
      vertLen = pcl_vert_len;
      fragCode = pcl_frag;
      fragLen = pcl_frag_len;
      pcSize = sizeof(PCLPushConstants);
      break;
    case ShaderType::MESH:
      vertCode = mesh_vert;
      vertLen = mesh_vert_len;
      fragCode = mesh_frag;
      fragLen = mesh_frag_len;
      pcSize = sizeof(MeshPushConstants);
      break;
    case ShaderType::EDGES:
      vertCode = edges_vert;
      vertLen = edges_vert_len;
      fragCode = edges_frag;
      fragLen = edges_frag_len;
      pcSize = sizeof(EdgesPushConstants);
      break;
    case ShaderType::PICKING:
      vertCode = picking_vert;
      vertLen = picking_vert_len;
      fragCode = picking_frag;
      fragLen = picking_frag_len;
      pcSize = sizeof(PickingPushConstants);
      break;
    case ShaderType::GRID:
      vertCode = grid_vert;
      vertLen = grid_vert_len;
      fragCode = grid_frag;
      fragLen = grid_frag_len;
      pcSize = sizeof(GridPushConstants);
      break;
  }

  VkShaderModule vertMod = createModule(vertCode, vertLen);
  VkShaderModule fragMod = createModule(fragCode, fragLen);
  if (vertMod == VK_NULL_HANDLE || fragMod == VK_NULL_HANDLE) {
    if (vertMod) vkDestroyShaderModule(ctx.device, vertMod, nullptr);
    if (fragMod) vkDestroyShaderModule(ctx.device, fragMod, nullptr);
    return false;
  }

  VkPushConstantRange pcRange{
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, pcSize};
  VkPipelineLayoutCreateInfo plci{};
  plci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  plci.pushConstantRangeCount = 1;
  plci.pPushConstantRanges = &pcRange;
  if (vkCheck(vkCreatePipelineLayout(ctx.device, &plci, nullptr, &m_layout),
              "vkCreatePipelineLayout") != VK_SUCCESS) {
    vkDestroyShaderModule(ctx.device, vertMod, nullptr);
    vkDestroyShaderModule(ctx.device, fragMod, nullptr);
    return false;
  }

  VkPipelineShaderStageCreateInfo stages[2]{};
  stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  stages[0].module = vertMod;
  stages[0].pName = "main";
  stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  stages[1].module = fragMod;
  stages[1].pName = "main";

  VkPipelineVertexInputStateCreateInfo vi{};
  vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vi.vertexBindingDescriptionCount = 1;
  vi.pVertexBindingDescriptions = &kBinding;
  vi.vertexAttributeDescriptionCount = 2;
  vi.pVertexAttributeDescriptions = kAttribs;

  VkPipelineInputAssemblyStateCreateInfo ia{};
  ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  ia.topology = topology;

  VkPipelineViewportStateCreateInfo vs{};
  vs.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  vs.viewportCount = 1;
  vs.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rs{};
  rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rs.polygonMode = VK_POLYGON_MODE_FILL;
  rs.cullMode = VK_CULL_MODE_NONE;
  rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rs.lineWidth = 1.0f;

  VkPipelineMultisampleStateCreateInfo ms{};
  ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineDepthStencilStateCreateInfo ds{};
  ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  ds.depthTestEnable = enableDepth ? VK_TRUE : VK_FALSE;
  ds.depthWriteEnable = enableDepth ? VK_TRUE : VK_FALSE;
  ds.depthCompareOp = VK_COMPARE_OP_LESS;

  VkPipelineColorBlendAttachmentState cba{};
  cba.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                       VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  if (enableBlend) {
    cba.blendEnable = VK_TRUE;
    cba.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    cba.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    cba.colorBlendOp = VK_BLEND_OP_ADD;
    cba.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    cba.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    cba.alphaBlendOp = VK_BLEND_OP_ADD;
  }
  VkPipelineColorBlendStateCreateInfo cb{};
  cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  cb.attachmentCount = 1;
  cb.pAttachments = &cba;

  VkDynamicState dynStates[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dyn{};
  dyn.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dyn.dynamicStateCount = 2;
  dyn.pDynamicStates = dynStates;

  VkGraphicsPipelineCreateInfo pci{};
  pci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pci.stageCount = 2;
  pci.pStages = stages;
  pci.pVertexInputState = &vi;
  pci.pInputAssemblyState = &ia;
  pci.pViewportState = &vs;
  pci.pRasterizationState = &rs;
  pci.pMultisampleState = &ms;
  pci.pDepthStencilState = &ds;
  pci.pColorBlendState = &cb;
  pci.pDynamicState = &dyn;
  pci.layout = m_layout;
  pci.renderPass = renderPass;
  pci.subpass = 0;

  bool ok = vkCheck(vkCreateGraphicsPipelines(ctx.device, VK_NULL_HANDLE, 1,
                                              &pci, nullptr, &m_pipeline),
                    "vkCreateGraphicsPipelines") == VK_SUCCESS;
  vkDestroyShaderModule(ctx.device, vertMod, nullptr);
  vkDestroyShaderModule(ctx.device, fragMod, nullptr);
  if (!ok) {
    vkDestroyPipelineLayout(ctx.device, m_layout, nullptr);
    m_layout = VK_NULL_HANDLE;
  }
  return ok;
}

}  // namespace zview
