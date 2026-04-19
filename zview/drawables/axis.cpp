#include "zview/drawables/axis.h"

#include <cstring>
#include <iostream>

#include "zview/drawables/shape_draw_visitor.h"
#include "zview/drawables/shape_init_visitor.h"
#include "zview/graphics_backend/shader.h"
#include "zview/params/params.h"

namespace zview {

Axis::Axis(const MVPmat& mvp)
    : types::Mesh("mesh"),
      m_mvp{mvp},
      m_locOnScreen{
          Eigen::Affine3f{Eigen::Translation3f(-0.90, -0.8, 0)}.matrix()} {}

bool Axis::init() {
  static constexpr std::uint8_t c{255U};
  v() = {
      {-s, -s, -s, c, c, c, c}, {s, -s, -s, c, 0, 0, c}, {s, s, -s, c, c, 0, c},
      {-s, s, -s, 0, c, 0, c},  {-s, -s, s, 0, 0, c, c}, {s, -s, s, c, 0, c, c},
      {s, s, s, c, c, c, c},    {-s, s, s, 0, c, c, c},  {d, 0, 0, c, 0, 0, c},
      {0, d, 0, 0, c, 0, c},    {0, 0, d, 0, 0, c, c},
  };
  f() = {
      {0, 1, 5},  {0, 5, 4},  {0, 4, 3}, {3, 4, 7}, {4, 5, 10}, {5, 6, 10},
      {6, 7, 10}, {7, 4, 10}, {3, 2, 9}, {2, 6, 9}, {6, 7, 9},  {7, 3, 9},
      {6, 5, 8},  {1, 5, 8},  {5, 6, 8}, {6, 2, 8},
  };
  return ShapeInitVisitor()(*this);
}

void Axis::draw(VkCommandBuffer cmd) const {
  static constexpr float rad_30 = static_cast<float>(M_PI) / 180.0f * 30.0f;
  Eigen::Affine3f r{m_mvp.getViewRotation()};
  float dist = (m_mvp.getViewDistance() * Params::i().zmin_factor) / d;
  r.pretranslate(types::Vector3{0, 0, -dist});
  float zoom_scale =
      std::tan(Params::i().camera_fov_rad / 2.0f) / std::tan(rad_30);
  r.scale(dist * zoom_scale);
  types::Matrix4x4 axisRotation = m_mvp.getProjectiveMatrix() * r.matrix();
  types::Matrix4x4 m = m_locOnScreen * axisRotation;

  MeshPushConstants pc{};
  std::memcpy(pc.mvp, m.data(), 64);
  pc.txt = 2;

  if (vertexBuffer == VK_NULL_HANDLE) return;
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader().pipeline());
  vkCmdPushConstants(cmd, shader().pipelineLayout(),
                     VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                     0, sizeof(pc), &pc);
  VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &offset);
  vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(cmd, (uint32_t)f().size() * 3, 1, 0, 0, 0);
}

}  // namespace zview
