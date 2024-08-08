#pragma once

#include <string>
#include <unordered_map>
namespace zview {

class Shader {
 public:
  Shader();
  enum class ShaderType : std::uint8_t { PCL, EDGES, MESH, PICKING, GRID };
  bool init(const ShaderType& shader_type);
  void use() const;
  void unuse() const;
  void setUniform(const char* name, const std::array<float, 3U>& val) const;
  template <typename T>
  void setUniform(const char* name, const T val) const;
  template <typename T>
  void setUniform(const char* name, const T val1, const T val2) const;
  template <typename T>
  void setUniform(const char* name, const T val1, const T val2,
                  const T val3) const;

 private:
  std::int32_t getLocation(const std::string& name) const;

  bool checkCompileErr();
  bool checkLinkingErr();
  bool compile(const std::string& vertex_code,
               const std::string& fragment_code);
  bool link();
  unsigned int m_vertex_id{0};
  unsigned int m_fragment_id{0};
  unsigned int m_id{0};
  mutable std::unordered_map<std::string, std::int32_t> m_location_key;
};
}  // namespace zview