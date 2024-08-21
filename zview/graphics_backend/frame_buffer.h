#pragma once
#include <array>
#include <cstdint>
namespace zview {
class FrameBuffer {
  struct ParamSet {
    std::int32_t internalFormat;
    std::int32_t format;
    std::int32_t type;
    std::int32_t filterType;
  };

 public:
  enum class TextureType : std::uint8_t { RGBA8, RGB32UI };
  FrameBuffer(TextureType textureType);
  ~FrameBuffer();
  FrameBuffer(const FrameBuffer &other) = delete;
  FrameBuffer &operator=(const FrameBuffer &other) = delete;
  FrameBuffer(FrameBuffer &&other) = delete;
  FrameBuffer &operator=(FrameBuffer &&other) = delete;

  bool resize(const std::array<int, 2> &wh);

  int width() const { return m_wh[0]; }
  int height() const { return m_wh[1]; }

  void bind();

  void unbind() const;

  std::uint32_t fbo() const { return m_fbo; }

  std::uint32_t txt() const { return m_txt; }

 private:
  void createTexture();
  ParamSet m_paramSet{};
  std::uint32_t m_txt{0};
  std::uint32_t m_fbo{0};
  std::uint32_t m_dpt{0};
  std::array<int, 2> m_wh{200, 200};

  std::array<std::int32_t, 4> m_viewport{0, 0, 0, 0};
};

}  // namespace zview