#include "zview/drawables/backdrop.h"

#include <GL/glew.h>  // Initialize with glewInit()
#include <GLFW/glfw3.h>

#include <limits>

#include "zview/drawables/shape_draw_visitor.h"
#include "zview/drawables/shape_init_visitor.h"
namespace zview {

Backdrop::Backdrop() : Mesh("backdrop") {}
bool zview::Backdrop::init(std::uint8_t background_color) {
  static const float zval = 1 - std::numeric_limits<float>::epsilon() * 1e3;
  auto f2i = [](float x) -> std::uint8_t {
    return static_cast<std::uint8_t>(x * 255);
  };

  switch (background_color) {
    {
      case 0:
        v() = {{-1.0f, -1.0f, zval, f2i(0.00), f2i(0.10), f2i(0.15)},
               {-1.0f, +1.0f, zval, f2i(0.03), f2i(0.21), f2i(0.26)},
               {+1.0f, -1.0f, zval, f2i(0.00), f2i(0.12), f2i(0.18)},
               {+1.0f, +1.0f, zval, f2i(0.06), f2i(0.26), f2i(0.30)}};
        break;
    }
    {
      case 1:
        v() = {{-1.0f, -1.0f, zval, f2i(0), f2i(0), f2i(0)},
               {-1.0f, +1.0f, zval, f2i(0), f2i(0), f2i(0)},
               {+1.0f, -1.0f, zval, f2i(0), f2i(0), f2i(0)},
               {+1.0f, +1.0f, zval, f2i(0), f2i(0), f2i(0)}};
        break;
    }
    case 2: {
      v() = {{-1.0f, -1.0f, zval, f2i(0.5), f2i(0.5), f2i(0.5)},
             {-1.0f, +1.0f, zval, f2i(0.5), f2i(0.5), f2i(0.5)},
             {+1.0f, -1.0f, zval, f2i(0.5), f2i(0.5), f2i(0.5)},
             {+1.0f, +1.0f, zval, f2i(0.5), f2i(0.5), f2i(0.5)}};
      break;
    }
    case 3: {
      v() = {{-1.0f, -1.0f, zval, f2i(1), f2i(1), f2i(1)},
             {-1.0f, +1.0f, zval, f2i(1), f2i(1), f2i(1)},
             {+1.0f, -1.0f, zval, f2i(1), f2i(1), f2i(1)},
             {+1.0f, +1.0f, zval, f2i(1), f2i(1), f2i(1)}};
      break;
    }
  }
  f() = {{0, 1, 2}, {1, 3, 2}};

  return ShapeInitVisitor()(*this);
}
void Backdrop::draw() const {
  shader().use();
  shader().setUniform("u_txt", 1);
  shader().setUniform("u_transformation", m_tformEye.data());
  ShapeDrawVisitor()(*this, nullptr);
}
}  // namespace zview
