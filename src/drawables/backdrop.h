#pragma once
#include "src/types/types.h"
namespace zview {
class Backdrop : public types::Mesh {
  const float m_tformEye[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

public:
  Backdrop();
  

  bool init();
  void draw() const;
};
} // namespace zview