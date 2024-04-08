#pragma once
#include <array>
#include <cstdint>
namespace zview {
struct Vertex {
  float x{0};
  float y{0};
  float z{0};
  std::uint8_t r{255U};
  std::uint8_t g{255U};
  std::uint8_t b{255U};
  std::uint8_t a{255U};
  constexpr Vertex() = default;
  constexpr Vertex(float x_, float y_, float z_, std::uint8_t r_ = 255U,
                   std::uint8_t g_ = 255U, std::uint8_t b_ = 255U,
                   std::uint8_t a_ = 255)
      : x(x_), y(y_), z(z_), r(r_), g(g_), b(b_), a(a_) {}
};
using Face = std::array<std::uint32_t, 3>;
using Edge = std::array<std::uint32_t, 2>;

}  // namespace zview