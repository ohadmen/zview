#include "zview/drawables/picking_texture.h"

#include <GL/glew.h>  // Initialize with glewInit()
#include <GLFW/glfw3.h>

namespace zview {
PickingTexture::PickingTexture()
    : FrameBuffer(FrameBuffer::TextureType::RGB32UI) {}
bool PickingTexture::resize(const std::array<int, 2> &wh) {
  if (!FrameBuffer::resize(wh)) {
    return false;
  }
  if (!m_pickingShader.init(Shader::ShaderType::PICKING)) {
    return false;
  }
  return true;
}

void PickingTexture::setObjectIndex(std::uint32_t object_index) const {
  m_pickingShader.setUniform("u_objectIndex", object_index);
}
void PickingTexture::setTransform(const types::Matrix4x4 &tform) const {
  m_pickingShader.setUniform("u_transformation", tform.data());
}

void PickingTexture::bind() {
  m_pickingShader.use();
  FrameBuffer::bind();
}

void PickingTexture::unbind() {
  m_pickingShader.unuse();
  FrameBuffer::unbind();
}

PickingTexture::PixelInfo PickingTexture::readPixel(int x, int y) const {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo());

  glReadBuffer(GL_COLOR_ATTACHMENT0);

  PixelInfo Pixel;
  // QUESTION: How does this correctly fill the PixelInfo struct? I would have
  // expected this is setting: valid = R channel, object_id = G channel, prim_id
  // = B channel
  // Or are we setting a different mask somewhere such that RGB does not refer
  // to color but valid, object_id and prim_id? Is this GL_COLOR_ATTACHMENT0?
  glReadPixels(x, height() - y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &Pixel);

  glReadBuffer(GL_NONE);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

  return Pixel;
}

// PickingTexture::~PickingTexture() {
//   if (m_fbo != 0) {
//     glDeleteFramebuffers(1, &m_fbo);
//   }

//   if (m_txt != 0) {
//     glDeleteTextures(1, &m_txt);
//   }

//   if (m_dpt != 0) {
//     glDeleteTextures(1, &m_dpt);
//   }
// }

}  // namespace zview
