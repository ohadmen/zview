#include "zview/drawables/grid.h"

#include <cstring>
#include <iostream>

#include "zview/drawables/shape_draw_visitor.h"
#include "zview/drawables/shape_init_visitor.h"
#include "zview/graphics_backend/vulkan_context.h"
#include "zview/params/params.h"

namespace zview {

Grid::Grid() : m_minor("minor") {}

bool Grid::init() {
  static constexpr float max_z_far = 1e4;
  static constexpr float max_fov_deg = 90;
  static const float q =
      std::pow(10.0f, 0.85f * std::log10(100 * max_z_far)) / 100 * 0.1f;
  static const float fp = max_z_far * tanf(max_fov_deg / 2 * M_PI / 180.0f) / q;
  static const float stride = 1.0f;
  static constexpr uint8_t col{255U};
  static constexpr uint8_t alpha{50};

  int n = static_cast<int>(fp / stride);
  for (int i = -n; i <= n; ++i) {
    float x = static_cast<float>(i) * stride;
    m_minor.v().push_back({x, -fp, 0, col, col, col, alpha});
    m_minor.v().push_back({x, fp, 0, col, col, col, alpha});
  }
  for (int i = -n; i <= n; ++i) {
    float y = static_cast<float>(i) * stride;
    m_minor.v().push_back({-fp, y, 0, col, col, col, alpha});
    m_minor.v().push_back({fp, y, 0, col, col, col, alpha});
  }
  for (unsigned int i = 0; i < m_minor.v().size(); i += 2)
    m_minor.e().push_back({i, i + 1});

  auto& ctx = VulkanContext::get();
  if (!m_shader.init(Shader::ShaderType::GRID, ctx.offscreenRenderPass,
                     /*blend=*/true, /*depth=*/true,
                     VK_PRIMITIVE_TOPOLOGY_LINE_LIST)) {
    std::cerr << "Failed to init grid shader\n";
    return false;
  }
  return ShapeInitVisitor()(m_minor);
}

void Grid::draw(VkCommandBuffer cmd, const types::Matrix4x4& mvp,
                const types::Vector3& model_loc, const float d) const {
  static constexpr float fov_factor = 0.25f;
  float camfov = std::tan(Params::i().camera_fov_rad / 2.0f) * d * 2;
  float q = std::pow(10.0f, std::floor(std::log10(camfov * fov_factor)));

  types::Vector3 shift{std::floor(-model_loc.x() / q) * q,
                       std::floor(-model_loc.y() / q) * q, 0};
  Eigen::Affine3f m{Eigen::Matrix4f::Identity()};
  m.scale(q);
  m.pretranslate(shift);
  types::Matrix4x4 mvp_grid = mvp * m.matrix();

  GridPushConstants pc{};
  std::memcpy(pc.mvp, mvp_grid.data(), 64);
  pc.shift[0] = shift.x();
  pc.shift[1] = shift.y();
  pc.scale = q;

  if (m_minor.vertexBuffer == VK_NULL_HANDLE) return;
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shader.pipeline());
  vkCmdPushConstants(cmd, m_shader.pipelineLayout(),
                     VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                     0, sizeof(pc), &pc);
  VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(cmd, 0, 1, &m_minor.vertexBuffer, &offset);
  vkCmdBindIndexBuffer(cmd, m_minor.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(cmd, (uint32_t)m_minor.e().size() * 2, 1, 0, 0, 0);
}

}  // namespace zview
