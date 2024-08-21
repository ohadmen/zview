#pragma once
#include <cstdint>

#include "zview/graphics_backend/frame_buffer.h"
#include "zview/graphics_backend/shader.h"
#include "zview/types/types.h"

namespace zview {
class PickingTexture : public FrameBuffer {
 public:
  PickingTexture();

  bool resize(const std::array<int, 2> &wh);

  void bind();

  void unbind();

  struct PixelInfo {
    std::uint32_t valid{0};
    std::uint32_t object_id{0};
    std::uint32_t prim_id{0};
  };

  PixelInfo readPixel(int x, int y) const;
  void setObjectIndex(std::uint32_t object_index) const;
  void setDrawIndex(std::uint32_t draw_index) const;
  void setTransform(const types::Matrix4x4 &tform) const;

 private:
  Shader m_pickingShader;
};
}  // namespace zview
