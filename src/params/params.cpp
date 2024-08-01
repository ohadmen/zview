#include "src/params/params.h"

#include <cmath>
namespace zview {
Params& Params::i() {
  static Params instance;
  return instance;
}
void Params::load(const std::string& fn) {}
float Params::deg2rad(float deg) {
  return deg * static_cast<float>(M_PI) / 180.0f;
}
}  // namespace zview
