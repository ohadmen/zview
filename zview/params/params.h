#pragma once
#include <array>
#include <string>
namespace zview {
class Params {
  static float deg2rad(float deg);

 private:
  Params() {}

 public:
  static Params &i();

  float camera_fov_rad{deg2rad(60.0f)};
  int texture_type{1};
  float point_size{0.1f};
  int background_color{0};
  std::array<float, 3> light_dir{0.4f, 0.48f, 0.51f};
  static constexpr float zmin_factor{1e-2f};
  static constexpr float zmax_factor{1e4f};
  float color_factor_shift{0.0f};
  float color_factor_scale{1.0f};
};
}  // namespace zview
