#include "zview/graphics_backend/frame_buffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
namespace zview {

FrameBuffer::FrameBuffer(TextureType textureType) {
  static constexpr ParamSet rgba8{GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE,
                                  GL_LINEAR};
  static constexpr ParamSet rgb32ui{GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT,
                                    GL_NEAREST};
  switch (textureType) {
    case TextureType::RGB32UI: {
      m_paramSet = rgb32ui;
      break;
    }
    case TextureType::RGBA8: {
      m_paramSet = rgba8;
      break;
    }
    default:
      throw std::runtime_error("Unknown texture type");
  }
}

bool FrameBuffer::resize(const std::array<int, 2> &wh) {
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
  glTexImage2D(GL_TEXTURE_2D, 0, m_paramSet.internalFormat, m_wh[0], m_wh[1], 0,
               m_paramSet.format, m_paramSet.type, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_paramSet.filterType);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_paramSet.filterType);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         m_txt, 0);

  // Create the texture object for the depth buffer
  if (m_dpt != 0) {
    glDeleteTextures(1, &m_dpt);
  }
  glGenTextures(1, &m_dpt);
  glBindTexture(GL_TEXTURE_2D, m_dpt);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, wh[0], wh[1], 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         m_dpt, 0);

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
  glViewport(0, 0, m_wh[0], m_wh[1]);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}
void FrameBuffer::unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
}
FrameBuffer::~FrameBuffer() {
  if (m_fbo != 0) {
    glDeleteFramebuffers(1, &m_fbo);
    m_fbo = 0;
  }

  if (m_txt != 0) {
    glDeleteTextures(1, &m_txt);
    m_txt = 0;
  }

  if (m_dpt != 0) {
    glDeleteTextures(1, &m_dpt);
    m_dpt = 0;
  }
}

}  // namespace zview