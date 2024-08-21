#include "zview/graphics_backend/shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <array>
#include <iostream>

#include "zview/graphics_backend/shader_code/edges_shader_code.h"
#include "zview/graphics_backend/shader_code/grid_shader_code.h"
#include "zview/graphics_backend/shader_code/mesh_shader_code.h"
#include "zview/graphics_backend/shader_code/pcl_shader_code.h"
#include "zview/graphics_backend/shader_code/picking_shader_code.h"

namespace zview {

Shader::Shader() {}
std::int32_t Shader::getLocation(const std::string &name) const {
  const auto ret = m_location_key.find(name);
  if (ret != m_location_key.end()) {
    return ret->second;
  }
  const auto location = glGetUniformLocation(m_id, name.c_str());
  m_location_key[name] = location;
  return location;
}

bool Shader::init(const ShaderType &shader_type) {
  std::string vertex_code;
  std::string fragment_code;
  switch (shader_type) {
    case ShaderType::PCL: {
      vertex_code = shader_code::pcl::vertex_shader;
      fragment_code = shader_code::pcl::fragment_shader;
      break;
    }
    case ShaderType::EDGES: {
      vertex_code = shader_code::edges::vertex_shader;
      fragment_code = shader_code::edges::fragment_shader;
      break;
    }
    case ShaderType::MESH: {
      vertex_code = shader_code::mesh::vertex_shader;
      fragment_code = shader_code::mesh::fragment_shader;
      break;
    }
    case ShaderType::PICKING: {
      vertex_code = shader_code::picking::vertex_shader;
      fragment_code = shader_code::picking::fragment_shader;
      break;
    }
    case ShaderType::GRID: {
      vertex_code = shader_code::grid::vertex_shader;
      fragment_code = shader_code::grid::fragment_shader;
      break;
    }
    default: {
      std::cerr << "Failed to init shader: " << static_cast<int>(shader_type)
                << std::endl;
      return false;
    }
  }

  bool ok = compile(vertex_code, fragment_code) && link();
  if (!ok) {
    std::cerr << "Failed to init shader: " << static_cast<int>(shader_type)
              << std::endl;
  }
  return ok;
}

bool Shader::compile(const std::string &vertex_code,
                     const std::string &fragment_code) {
  const char *vcode = vertex_code.c_str();
  m_vertex_id = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(m_vertex_id, 1, &vcode, NULL);
  glCompileShader(m_vertex_id);

  const char *fcode = fragment_code.c_str();
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
void Shader::unuse() const { glUseProgram(0); }

template <>
void Shader::setUniform<>(const char *name, const int val) const {
  glUniform1i(getLocation(name), val);
}
template <>
void Shader::setUniform<>(const char *name, const std::uint32_t val) const {
  glUniform1ui(getLocation(name), val);
}

template <>
void Shader::setUniform<>(const char *name, const bool val) const {
  glUniform1i(getLocation(name), val);
}

template <>
void Shader::setUniform<>(const char *name, const float val) const {
  glUniform1f(getLocation(name), val);
}

void Shader::setUniform(const char *name,
                        const std::array<float, 3U> &val) const {
  glUniform3f(getLocation(name), val.at(0), val.at(1), val.at(2));
}
template <>
void Shader::setUniform(const char *name, const float val1,
                        const float val2) const {
  glUniform2f(getLocation(name), val1, val2);
}

template <>
void Shader::setUniform<>(const char *name, const float *val) const {
  glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, val);
}

bool Shader::checkCompileErr() {
  int success{0};
  std::array<char, 1024> infoLog{};
  glGetShaderiv(m_vertex_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(m_vertex_id, 1024, NULL, infoLog.data());
    std::cout << "Error compiling Vertex Shader:\n"
              << std::string(infoLog.data()) << std::endl;
    return false;
  }
  glGetShaderiv(m_fragment_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(m_fragment_id, 1024, NULL, infoLog.data());
    std::cout << "Error compiling Fragment Shader:\n"
              << std::string(infoLog.data()) << std::endl;
    return false;
  }
  return true;
}

bool Shader::checkLinkingErr() {
  int success{0};
  std::array<char, 1024> infoLog{};
  glGetProgramiv(m_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(m_id, 1024, NULL, infoLog.data());
    std::cout << "Error Linking Shader Program:\n"
              << std::string(infoLog.data()) << std::endl;
    return false;
  }
  return true;
}
}  // namespace zview