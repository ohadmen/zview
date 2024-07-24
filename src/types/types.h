#pragma once
#include "src/graphics_backend/opengl_shader.h"
#include <Eigen/Dense>
#include <array>
#include <cmath>
#include <optional>
#include <variant>
#include <vector>
namespace zview {
namespace types {
using Vector2 = Eigen::Vector2f;
using Vector3 = Eigen::Vector3f;
using Vector4 = Eigen::Vector4f;
using Matrix3x3 = Eigen::Matrix3f;
using Matrix4x4 = Eigen::Matrix4f;
using Transform = Eigen::Transform<float, 3, Eigen::Affine>;

class Bbox3d {
  types::Vector3 m_min;
  types::Vector3 m_max;

public:
  Bbox3d() : m_min{0, 0, 0}, m_max{0, 0, 0} {}
  Bbox3d(const types::Vector3 &mmin, const types::Vector3 &mmax)
      : m_min(mmin), m_max(mmax) {}

  void operator|=(const Bbox3d &other) {
    m_min = m_min.cwiseMin(other.m_min);
    m_max = m_max.cwiseMax(other.m_max);
  }
  types::Vector3 min() const { return m_min; }
  types::Vector3 max() const { return m_max; }
  types::Vector3 &min() { return m_min; }
  types::Vector3 &max() { return m_max; }
  template<typename T>
  void applyTransform(const T &t) {
    m_min = t * m_min;
    m_max = t * m_max;
  }
};

struct VertData {
  float x{0};
  float y{0};
  float z{0};
  std::uint8_t r{255U};
  std::uint8_t g{255U};
  std::uint8_t b{255U};
  std::uint8_t a{255U};

  constexpr VertData() {}
  constexpr VertData(float x_, float y_, float z_, uint8_t r_ = 255U,
                     uint8_t g_ = 255U, uint8_t b_ = 255U, uint8_t a_ = 255)
      : x(x_), y(y_), z(z_), r(r_), g(g_), b(b_), a(a_) {}
  VertData(const Vector3 &xyz,
           const Vector4 &rgba = Vector4{255, 255, 255, 255})
      : x(xyz[0]), y(xyz[1]), z(xyz[2]), r(rgba.x()), g(rgba.y()), b(rgba.z()),
        a(rgba.w()) {}

  operator Vector3() const { return Vector3(x, y, z); }
  operator Vector4() const { return Vector4(r, g, b, a); }
  bool operator!=(const VertData &rhs) const {
    return x != rhs.x || y != rhs.y || z != rhs.z;
  }
  float operator[](int i) {
    return i == 0   ? x
           : i == 1 ? y
           : i == 2 ? z
           : y == 3 ? r
           : y == 4 ? g
           : y == 5 ? b
           : y == 6 ? a
                    : std::numeric_limits<float>::quiet_NaN();
  }
  bool operator<(const VertData &rhs) const {
    if (x != rhs.x)
      return x < rhs.x;
    else if (y != rhs.y)
      return y < rhs.y;
    else if (z != rhs.z)
      return z < rhs.z;
    else
      return false;
  }
};
using FaceIndx = std::array<uint32_t, 3>;
using EdgeIndx = std::array<uint32_t, 2>;

class Pcl {

std::string m_name{};
protected:
  
  std::vector<types::VertData> m_v{};
  std::uint32_t m_vbo{0};
  std::uint32_t m_vao{0};
  // init shader
  Shader m_shader{};
  bool m_enabled{true};

public:
  std::uint32_t &vbo() { return m_vbo; }
  std::uint32_t &vao() { return m_vao; }

  std::uint32_t vbo() const { return m_vbo; }
  std::uint32_t vao() const { return m_vao; }

  Pcl(const std::string &name) : m_name(name) {}
  const std::string &getName() const { return m_name; }
  void setName(const std::string &name) { m_name = name; }

  std::vector<VertData> &v() { return m_v; }
  const std::vector<VertData> &v() const { return m_v; }
  bool initShader(const std::string &shader_name);
  const Shader &shader() const;
  Bbox3d getBbox() const;
  virtual std::optional<types::Vector3>
  get3dLocation(const std::uint32_t &prim_index,
                const std::array<types::Vector3, 2> &ray) const;
  
  bool& enabled() { return m_enabled;}
  bool enabled() const { return m_enabled;}
  
};

class Mesh : public Pcl {
private:
  std::uint32_t m_ebo;

protected:
  std::vector<FaceIndx> m_f;

public:
  std::uint32_t &ebo() { return m_ebo; }
  std::uint32_t ebo() const { return m_ebo; }

  Mesh(const std::string &name) : Pcl(name) {}
  std::vector<FaceIndx> &f() { return m_f; }
  const std::vector<FaceIndx> &f() const { return m_f; }
  std::optional<types::Vector3>
  get3dLocation(const std::uint32_t &prim_index,
                const std::array<types::Vector3, 2> &ray) const override;
};

class Edges : public Pcl {
private:
  std::uint32_t m_ebo;

protected:
  std::vector<EdgeIndx> m_e;

public:
  std::uint32_t &ebo() { return m_ebo; }
  std::uint32_t ebo() const { return m_ebo; }

  Edges(const std::string &name) : Pcl(name) {}
  std::vector<EdgeIndx> &e() { return m_e; }
  const std::vector<EdgeIndx> &e() const { return m_e; }

  std::optional<types::Vector3>
  get3dLocation(const std::uint32_t &prim_index,
                const std::array<types::Vector3, 2> &ray) const override;
};
using Shape = std::variant<Pcl, Edges, Mesh>;
} // namespace types
} // namespace zview