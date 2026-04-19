#include "zview/drawables/picking_texture.h"

#include <cstring>
#include <type_traits>

#include "zview/graphics_backend/vulkan_context.h"

namespace zview {

PickingTexture::PickingTexture()
    : FrameBuffer(FrameBuffer::TextureType::RGBA32UI) {}

PickingTexture::~PickingTexture() {
  auto& ctx = VulkanContext::get();
  if (ctx.allocator && m_readbackBuf) {
    vmaDestroyBuffer(ctx.allocator, m_readbackBuf, m_readbackAlloc);
  }
}

bool PickingTexture::resize(const std::array<int, 2>& wh) {
  auto& ctx = VulkanContext::get();
  if (!FrameBuffer::resize(wh)) return false;
  if (!m_pickingShader.init(Shader::ShaderType::PICKING, ctx.pickingRenderPass,
                            false, true, VK_PRIMITIVE_TOPOLOGY_POINT_LIST))
    return false;
  if (!m_meshPickShader.init(Shader::ShaderType::PICKING, ctx.pickingRenderPass,
                             false, true, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST))
    return false;
  if (!m_edgesPickShader.init(Shader::ShaderType::PICKING,
                              ctx.pickingRenderPass, false, true,
                              VK_PRIMITIVE_TOPOLOGY_LINE_LIST))
    return false;

  // Recreate readback buffer for the new size
  if (m_readbackBuf) {
    vmaDestroyBuffer(ctx.allocator, m_readbackBuf, m_readbackAlloc);
    m_readbackBuf = VK_NULL_HANDLE;
  }
  VkDeviceSize bufSize = (VkDeviceSize)wh[0] * wh[1] * 4 * sizeof(uint32_t);
  VkBufferCreateInfo bci{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
  bci.size = bufSize;
  bci.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  VmaAllocationCreateInfo aci{};
  aci.usage = VMA_MEMORY_USAGE_CPU_ONLY;
  vmaCreateBuffer(ctx.allocator, &bci, &aci, &m_readbackBuf, &m_readbackAlloc,
                  nullptr);
  m_readbackW = wh[0];
  m_readbackH = wh[1];
  return m_readbackBuf != VK_NULL_HANDLE;
}

void PickingTexture::bind(VkCommandBuffer cmd) { FrameBuffer::bind(cmd); }

void PickingTexture::unbind(VkCommandBuffer cmd) {
  FrameBuffer::unbind(cmd);
  // Picking render pass finalLayout is TRANSFER_SRC_OPTIMAL; copy image →
  // readback buffer Transition from TRANSFER_SRC (set by render pass
  // finalLayout) is already done.
  VkBufferImageCopy region{};
  region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
  region.imageExtent = {(uint32_t)m_readbackW, (uint32_t)m_readbackH, 1};
  vkCmdCopyImageToBuffer(cmd, colorImage(),
                         VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_readbackBuf, 1,
                         &region);
}

void PickingTexture::drawPicking(VkCommandBuffer cmd, VkPipeline pipeline,
                                 VkPipelineLayout layout,
                                 const PickingPushConstants& pc, VkBuffer vbuf,
                                 uint32_t vtxCount, VkBuffer ibuf,
                                 uint32_t idxCount) {
  if (vbuf == VK_NULL_HANDLE || vtxCount == 0) return;
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdPushConstants(cmd, layout,
                     VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                     0, sizeof(pc), &pc);
  VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(cmd, 0, 1, &vbuf, &offset);
  if (ibuf != VK_NULL_HANDLE && idxCount > 0) {
    vkCmdBindIndexBuffer(cmd, ibuf, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmd, idxCount, 1, 0, 0, 0);
  } else {
    vkCmdDraw(cmd, vtxCount, 1, 0, 0);
  }
}

void PickingTexture::drawShapeForPicking(VkCommandBuffer cmd,
                                         const types::Shape& shape,
                                         uint32_t objectIndex,
                                         const types::Matrix4x4& mvp) {
  PickingPushConstants pc{};
  std::memcpy(pc.mvp, mvp.data(), 64);
  pc.objectIndex = objectIndex;

  std::visit(
      [&](const auto& s) {
        using T = std::decay_t<decltype(s)>;
        if (s.vertexBuffer == VK_NULL_HANDLE || s.v().empty()) return;
        if constexpr (std::is_same_v<T, types::Pcl>) {
          drawPicking(cmd, m_pickingShader.pipeline(),
                      m_pickingShader.pipelineLayout(), pc, s.vertexBuffer,
                      (uint32_t)s.v().size(), VK_NULL_HANDLE, 0);
        } else if constexpr (std::is_same_v<T, types::Mesh>) {
          drawPicking(cmd, m_meshPickShader.pipeline(),
                      m_meshPickShader.pipelineLayout(), pc, s.vertexBuffer,
                      (uint32_t)s.v().size(), s.indexBuffer,
                      (uint32_t)s.f().size() * 3);
        } else if constexpr (std::is_same_v<T, types::Edges>) {
          drawPicking(cmd, m_edgesPickShader.pipeline(),
                      m_edgesPickShader.pipelineLayout(), pc, s.vertexBuffer,
                      (uint32_t)s.v().size(), s.indexBuffer,
                      (uint32_t)s.e().size() * 2);
        }
      },
      shape);
}

PickingTexture::PixelInfo PickingTexture::readPixel(int x, int y) const {
  if (m_readbackBuf == VK_NULL_HANDLE || x < 0 || y < 0 || x >= m_readbackW ||
      y >= m_readbackH) {
    return {};
  }
  // Wait for GPU copy to complete (submitted in same command buffer, fence
  // waited by caller)
  void* mapped = nullptr;
  vmaMapMemory(VulkanContext::get().allocator, m_readbackAlloc, &mapped);
  // Each pixel is uvec4 = 4 x uint32. Row is flipped: Vulkan Y-down vs OpenGL
  // Y-up.
  int flippedY = m_readbackH - 1 - y;
  const uint32_t* pixels = reinterpret_cast<const uint32_t*>(mapped);
  const uint32_t* p = pixels + (flippedY * m_readbackW + x) * 4;
  PixelInfo info{p[0], p[1], p[2]};
  vmaUnmapMemory(VulkanContext::get().allocator, m_readbackAlloc);
  return info;
}

}  // namespace zview
