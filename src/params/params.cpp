#include "params.h"

#include <cmath>
namespace zview {
Params &Params::i() {
  static Params instance;
  return instance;
}
void Params::load(std::string fn) {}
float Params::deg2rad(float deg) { return deg * M_PIf / 180.0f; };
}  // namespace zview