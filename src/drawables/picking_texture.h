#pragma once
#include <cstdint>

#include "src/graphics_backend/shader.h"
#include "src/types/types.h"

namespace zview {
class PickingTexture {
 public:
  PickingTexture() {}
  // copy constructor
  PickingTexture(const PickingTexture &other) = delete;
  // copy assignment
  PickingTexture &operator=(const PickingTexture &other) = delete;
  // move constructor
  PickingTexture(PickingTexture &&other) = delete;
  // move assignment
  PickingTexture &operator=(PickingTexture &&other) = delete;

  // destructor
  ~PickingTexture();

  bool init(const std::array<int, 2> &wh);

  void enableWriting() const;

  void disableWriting() const;

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
  std::uint32_t m_fbo{0};
  std::uint32_t m_txt{0};
  std::uint32_t m_depthTexture{0};
  std::array<std::int32_t, 2> m_wh{0, 0};
  std::array<std::int32_t, 2> m_xy{0, 0};
  mutable std::array<std::int32_t, 4> m_viewportSave{0, 0, 0, 0};
};
}  // namespace zview
