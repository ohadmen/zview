#include "opengl_shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "src/io/read_file.h"

Shader::Shader() {}
std::int32_t Shader::getLocation(const std::string& name) const {
  const auto ret = m_location_key.find(name);
  if (ret != m_location_key.end()) {
    return ret->second;
  }
  const auto location = glGetUniformLocation(m_id, name.c_str());
  m_location_key[name] = location;
  return location;
}

bool Shader::init(const std::string& shader_name) {
  const std::string vertex_code = readFile("shaders/" + shader_name + ".vs");
  const std::string fragment_code = readFile("shaders/" + shader_name + ".fs");
  m_vertex_code = vertex_code;
  m_fragment_code = fragment_code;
  bool fail = !compile() || !link();

  return !fail;
}

bool Shader::compile() {
  const char* vcode = m_vertex_code.c_str();
  m_vertex_id = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(m_vertex_id, 1, &vcode, NULL);
  glCompileShader(m_vertex_id);

  const char* fcode = m_fragment_code.c_str();
  m_fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(m_fragment_id, 1, &fcode, NULL);
  glCompileShader(m_fragment_id);
  return checkCompileErr();
}

bool Shader::link() {
  m_id = glCreateProgram();
  glAttachShader(m_id, m_vertex_id);
  glAttachShader(m_id, m_fragment_id);
  glLinkProgram(m_id);
  bool ok = checkLinkingErr();
  glDeleteShader(m_vertex_id);
  glDeleteShader(m_fragment_id);
  return ok;
}

void Shader::use() const { glUseProgram(m_id); }

template <>
void Shader::setUniform<>(const std::string& name, const int val) const {
  glUniform1i(getLocation(name), val);
}
template <>
void Shader::setUniform<>(const std::string& name,
                          const std::uint32_t val) const {
  glUniform1ui(getLocation(name), val);
}

template <>
void Shader::setUniform<>(const std::string& name, const bool val) const {
  glUniform1i(getLocation(name), val);
}

template <>
void Shader::setUniform<>(const std::string& name, const float val) const {
  glUniform1f(getLocation(name), val);
}

void Shader::setUniform(const std::string& name,
                        const std::array<float, 3U>& val) const {
  glUniform3f(getLocation(name), val[0], val[1], val[2]);
}

template <>
void Shader::setUniform<>(const std::string& name, const float* val) const {
  glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, val);
}

bool Shader::checkCompileErr() {
  int success;
  char infoLog[1024];
  glGetShaderiv(m_vertex_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(m_vertex_id, 1024, NULL, infoLog);
    std::cout << "Error compiling Vertex Shader:\n" << infoLog << std::endl;
    return false;
  }
  glGetShaderiv(m_fragment_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(m_fragment_id, 1024, NULL, infoLog);
    std::cout << "Error compiling Fragment Shader:\n" << infoLog << std::endl;
    return false;
  }
  return true;
}

bool Shader::checkLinkingErr() {
  int success;
  char infoLog[1024];
  glGetProgramiv(m_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(m_id, 1024, NULL, infoLog);
    std::cout << "Error Linking Shader Program:\n" << infoLog << std::endl;
    return false;
  }
  return true;
}
