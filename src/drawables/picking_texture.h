#pragma once
#include <cstdint>

#include "src/graphics_backend/opengl_shader.h"
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

  PixelInfo readPixel(unsigned int x, unsigned int y) const;
  void setObjectIndex(std::uint32_t object_index) const;
  void setDrawIndex(std::uint32_t draw_index) const;
  void setTransform(const types::Matrix4x4 &tform) const;

 private:
  std::uint32_t m_height{0};
  Shader m_pickingShader;
  std::uint32_t m_fbo{0};
  std::uint32_t m_pickingTexture{0};
  std::uint32_t m_depthTexture{0};
};
}  // namespace zview
