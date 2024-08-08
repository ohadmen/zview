#include "src/graphics_backend/frame_buffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
namespace zview {

FrameBuffer::FrameBuffer(){

};

bool FrameBuffer::init(const std::array<int, 2> &wh) {
  if (wh == m_wh) {
    return true;
  }
  m_wh = wh;

  if (m_fbo != 0) {
    glDeleteFramebuffers(1, &m_fbo);
  }
  glGenFramebuffers(1, &m_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  if (m_txt != 0) {
    glDeleteTextures(1, &m_txt);
  }
  glGenTextures(1, &m_txt);
  glBindTexture(GL_TEXTURE_2D, m_txt);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_wh[0], m_wh[1], 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         m_txt, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (Status != GL_FRAMEBUFFER_COMPLETE) {
    return false;
  }
  bool ok = glGetError() == GL_NO_ERROR;
  return ok;
}
void FrameBuffer::bind() {
  glGetIntegerv(GL_VIEWPORT, m_viewport.data());
  glViewport(m_xy[0], m_xy[1], m_wh[0], m_wh[1]);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT);
}
void FrameBuffer::unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
}
FrameBuffer::~FrameBuffer() {
  glDeleteFramebuffers(1, &m_fbo);
  glDeleteTextures(1, &m_txt);
}

}  // namespace zview