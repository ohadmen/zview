#include "zview/drawables/shape_draw_visitor.h"

#include <cstring>

#include "zview/graphics_backend/shader.h"
#include "zview/params/params.h"

namespace zview {

static void bindAndDraw(VkCommandBuffer cmd, const types::Pcl& obj,
                        VkPipeline pipeline, VkPipelineLayout layout,
                        const void* pc, uint32_t pcSize) {
  if (obj.vertexBuffer == VK_NULL_HANDLE || obj.v().empty()) return;
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdPushConstants(cmd, layout,
                     VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                     0, pcSize, pc);
  VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(cmd, 0, 1, &obj.vertexBuffer, &offset);
  vkCmdDraw(cmd, (uint32_t)obj.v().size(), 1, 0, 0);
}

static void bindAndDrawIndexed(VkCommandBuffer cmd, const types::Pcl& obj,
                               VkBuffer indexBuffer, uint32_t indexCount,
                               VkPipeline pipeline, VkPipelineLayout layout,
                               const void* pc, uint32_t pcSize) {
  if (obj.vertexBuffer == VK_NULL_HANDLE || indexBuffer == VK_NULL_HANDLE)
    return;
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdPushConstants(cmd, layout,
                     VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                     0, pcSize, pc);
  VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(cmd, 0, 1, &obj.vertexBuffer, &offset);
  vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(cmd, indexCount, 1, 0, 0, 0);
}

void ShapeDrawVisitor::operator()(const types::Pcl& obj, VkCommandBuffer cmd,
                                  const float* tform) const {
  PCLPushConstants pc{};
  if (tform) {
    std::memcpy(pc.mvp, tform, 64);
    // Compute near plane distance from viewport (approximate via ptsize)
    pc.ptsize = zview::Params::i().point_size;
    pc.nearPlaneDist =
        500.0f;  // reasonable default; exact value set per-frame below
    pc.txt = zview::Params::i().texture_type;
    auto ld = zview::Params::i().light_dir;
    pc.lightDir[0] = ld[0];
    pc.lightDir[1] = ld[1];
    pc.lightDir[2] = ld[2];
    pc.lightDir[3] = 0;
  }
  bindAndDraw(cmd, obj, obj.shader().pipeline(), obj.shader().pipelineLayout(),
              &pc, sizeof(pc));
}

void ShapeDrawVisitor::operator()(const types::Edges& obj, VkCommandBuffer cmd,
                                  const float* tform) const {
  EdgesPushConstants pc{};
  if (tform) std::memcpy(pc.mvp, tform, 64);
  const auto& e = obj;
  bindAndDrawIndexed(cmd, obj, e.indexBuffer, (uint32_t)e.e().size() * 2,
                     obj.shader().pipeline(), obj.shader().pipelineLayout(),
                     &pc, sizeof(pc));
}

void ShapeDrawVisitor::operator()(const types::Mesh& obj, VkCommandBuffer cmd,
                                  const float* tform) const {
  MeshPushConstants pc{};
  if (tform) {
    std::memcpy(pc.mvp, tform, 64);
    pc.txt = zview::Params::i().texture_type;
    auto ld = zview::Params::i().light_dir;
    pc.lightDir[0] = ld[0];
    pc.lightDir[1] = ld[1];
    pc.lightDir[2] = ld[2];
    pc.lightDir[3] = 0;
  }
  bindAndDrawIndexed(cmd, obj, obj.indexBuffer, (uint32_t)obj.f().size() * 3,
                     obj.shader().pipeline(), obj.shader().pipelineLayout(),
                     &pc, sizeof(pc));
}

}  // namespace zview
