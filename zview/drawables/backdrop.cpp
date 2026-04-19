#include "zview/drawables/backdrop.h"

#include <cstring>
#include <limits>

#include "zview/drawables/shape_draw_visitor.h"
#include "zview/drawables/shape_init_visitor.h"
#include "zview/graphics_backend/shader.h"

namespace zview {

Backdrop::Backdrop() : Mesh("backdrop") {}

bool Backdrop::init(std::uint8_t background_color) {
  static const float zval = 1 - std::numeric_limits<float>::epsilon() * 1e3;
  auto f2i = [](float x) -> std::uint8_t {
    return static_cast<std::uint8_t>(x * 255);
  };

  switch (background_color) {
    case 0:
      v() = {{-1.f, -1.f, zval, f2i(0.00f), f2i(0.10f), f2i(0.15f)},
             {-1.f, +1.f, zval, f2i(0.03f), f2i(0.21f), f2i(0.26f)},
             {+1.f, -1.f, zval, f2i(0.00f), f2i(0.12f), f2i(0.18f)},
             {+1.f, +1.f, zval, f2i(0.06f), f2i(0.26f), f2i(0.30f)}};
      break;
    case 1:
      v() = {{-1.f, -1.f, zval, 0, 0, 0},
             {-1.f, +1.f, zval, 0, 0, 0},
             {+1.f, -1.f, zval, 0, 0, 0},
             {+1.f, +1.f, zval, 0, 0, 0}};
      break;
    case 2:
      v() = {{-1.f, -1.f, zval, 128, 128, 128},
             {-1.f, +1.f, zval, 128, 128, 128},
             {+1.f, -1.f, zval, 128, 128, 128},
             {+1.f, +1.f, zval, 128, 128, 128}};
      break;
    case 3:
    default:
      v() = {{-1.f, -1.f, zval, 255, 255, 255},
             {-1.f, +1.f, zval, 255, 255, 255},
             {+1.f, -1.f, zval, 255, 255, 255},
             {+1.f, +1.f, zval, 255, 255, 255}};
      break;
  }
  f() = {{0, 1, 2}, {1, 3, 2}};
  return ShapeInitVisitor()(*this);
}

void Backdrop::draw(VkCommandBuffer cmd) const {
  MeshPushConstants pc{};
  std::memcpy(pc.mvp, m_tformEye.data(), 64);
  pc.txt = 1;
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
