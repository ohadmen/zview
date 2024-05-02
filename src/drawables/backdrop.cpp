#include "src/drawables/backdrop.h"
#include "src/drawables/shape_draw_visitor.h"
#include "src/drawables/shape_init_visitor.h"
#include "src/params/params.h"


#include <GL/glew.h> // Initialize with glewInit()
#include <GLFW/glfw3.h>
namespace zview {

Backdrop::Backdrop(): Mesh("backdrop") {}
bool zview::Backdrop::init() {
  static const float zval = 1 - std::numeric_limits<float>::epsilon() * 1e3;
  auto f = [](float x) -> std::uint8_t { return std::uint8_t(x * 255); };
  
  switch (Params::i().background_color) {
    {
    case 0:
      m_v = {{-1.0f, -1.0f, zval, f(0.00), f(0.10), f(0.15)},
             {-1.0f, +1.0f, zval, f(0.03), f(0.21), f(0.26)},
             {+1.0f, -1.0f, zval, f(0.00), f(0.12), f(0.18)},
             {+1.0f, +1.0f, zval, f(0.06), f(0.26), f(0.30)}};
      break;
    }
    {
    case 1:
      m_v = {{-1.0f, -1.0f, zval, f(0), f(0), f(0)},
             {-1.0f, +1.0f, zval, f(0), f(0), f(0)},
             {+1.0f, -1.0f, zval, f(0), f(0), f(0)},
             {+1.0f, +1.0f, zval, f(0), f(0), f(0)}};
      break;
    }
    case 2:
    {
      m_v = {{-1.0f, -1.0f, zval, f(0.5), f(0.5), f(0.5)},
             {-1.0f, +1.0f, zval, f(0.5), f(0.5), f(0.5)},
             {+1.0f, -1.0f, zval, f(0.5), f(0.5), f(0.5)},
             {+1.0f, +1.0f, zval, f(0.5), f(0.5), f(0.5)}};
      break;
    }
    case 3:
    {
      m_v = {{-1.0f, -1.0f, zval, f(1), f(1), f(1)},
             {-1.0f, +1.0f, zval, f(1), f(1), f(1)},
             {+1.0f, -1.0f, zval, f(1), f(1), f(1)},
             {+1.0f, +1.0f, zval, f(1), f(1), f(1)}};
      break;
    }
  }
  m_f = {{0, 1, 2}, {1, 3, 2}};

  return ShapeInitVisitor()(*this);
}
void Backdrop::draw() const {
  m_shader.use();
  m_shader.setUniform("u_txt", 1);
  m_shader.setUniform("u_transformation", m_tformEye);
  ShapeDrawVisitor()(*this, nullptr);
}
} // namespace zview