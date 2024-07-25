#include "src/drawables/picking_texture.h"

#include <GL/glew.h>  // Initialize with glewInit()
#include <GLFW/glfw3.h>

namespace zview {
bool PickingTexture::init(const std::array<int, 2> &wh) {
  // Create the FBO
  if (m_fbo != 0) {
    glDeleteFramebuffers(1, &m_fbo);
  }
  m_pickingShader.init("picking");
  m_height = wh[1];
  glGenFramebuffers(1, &m_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

  // Create the texture object for the primitive information buffer
  glGenTextures(1, &m_pickingTexture);
  glBindTexture(GL_TEXTURE_2D, m_pickingTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, wh[0], wh[1], 0, GL_RGB_INTEGER,
               GL_UNSIGNED_INT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         m_pickingTexture, 0);

  // Create the texture object for the depth buffer
  if (m_depthTexture != 0) {
    glDeleteTextures(1, &m_depthTexture);
  }
  glGenTextures(1, &m_depthTexture);
  glBindTexture(GL_TEXTURE_2D, m_depthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, wh[0], wh[1], 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         m_depthTexture, 0);

  // Verify that the FBO is correct
  GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (Status != GL_FRAMEBUFFER_COMPLETE) {
    return false;
  }

  // Restore the default framebuffer
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  bool ok = glGetError() == GL_NO_ERROR;
  return ok;
}

void PickingTexture::setObjectIndex(std::uint32_t object_index) const {
  m_pickingShader.setUniform("u_objectIndex", object_index);
}
void PickingTexture::setTransform(const types::Matrix4x4 &tform) const {
  m_pickingShader.setUniform("u_transformation", tform.data());
}

void PickingTexture::enableWriting() const {
  m_pickingShader.use();
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void PickingTexture::disableWriting() const {
  // Bind back the default framebuffer
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

PickingTexture::PixelInfo PickingTexture::readPixel(int x, int y) const {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);

  glReadBuffer(GL_COLOR_ATTACHMENT0);

  PixelInfo Pixel;
  glReadPixels(x, m_height - y - 1, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT,
               &Pixel);

  glReadBuffer(GL_NONE);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

  return Pixel;
}
PickingTexture::~PickingTexture() {
  if (m_fbo != 0) {
    glDeleteFramebuffers(1, &m_fbo);
  }

  if (m_pickingTexture != 0) {
    glDeleteTextures(1, &m_pickingTexture);
  }

  if (m_depthTexture != 0) {
    glDeleteTextures(1, &m_depthTexture);
  }
}
}  // namespace zview
