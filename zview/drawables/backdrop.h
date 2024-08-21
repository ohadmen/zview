#pragma once
#include "zview/types/types.h"
namespace zview {
class Backdrop : public types::Mesh {
  const std::array<float, 16> m_tformEye = {1, 0, 0, 0, 0, 1, 0, 0,
                                            0, 0, 1, 0, 0, 0, 0, 1};

 public:
  Backdrop();

  bool init(std::uint8_t background_color);
  void draw() const;
};
}  // namespace zview
