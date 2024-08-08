#pragma once
#include <array>
#include <cstdint>
namespace zview {
class FrameBuffer {
 public:
  FrameBuffer();
  ~FrameBuffer();
  FrameBuffer(const FrameBuffer &other) = delete;
  FrameBuffer &operator=(const FrameBuffer &other) = delete;
  FrameBuffer(FrameBuffer &&other) = delete;
  FrameBuffer &operator=(FrameBuffer &&other) = delete;

  bool init(const std::array<int, 2> &wh);

  void bind();

  void setXY(const std::array<int, 2> &xy) { m_xy = xy; }

  void unbind() const;

  std::uint32_t txt() const { return m_txt; }

 private:
  std::uint32_t m_txt{0};
  std::uint32_t m_fbo{0};
  std::array<int, 2> m_wh{200, 200};
  std::array<int, 2> m_xy{0, 0};
  std::array<std::int32_t, 4> m_viewport{0, 0, 0, 0};
};

}  // namespace zview