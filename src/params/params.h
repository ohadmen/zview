#pragma once
#include <array>
#include <string>
namespace zview {
class Params {
  static float deg2rad(float deg);

 private:
  Params() {}

 public:
  static Params& i();

  void load(const std::string& fn);

  float camera_fov_rad{deg2rad(60.0f)};
  float camera_z_near{1e-1f};
  float camera_z_far{1e4f};
  int texture_type{1};
  float point_size{3.0f};
  int background_color{0};
  std::array<float, 3> light_dir{0.4f, 0.48f, 0.51f};
};
}  // namespace zview
