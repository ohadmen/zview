#pragma once
#include <Eigen/Dense>
#include <array>
#include <cmath>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "zview/graphics_backend/shader.h"
#include "zview/types/external_types.h"

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
  // ignore clang-tidy
  //  NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  Bbox3d(const types::Vector3 &mmin, const types::Vector3 &mmax)
      : m_min(mmin), m_max(mmax) {}

  types::Vector3 min() const { return m_min; }
  types::Vector3 max() const { return m_max; }
  types::Vector3 &min() { return m_min; }
  types::Vector3 &max() { return m_max; }
  std::array<types::Vector3, 8> getVertices() const;
};

class VertData : public Vertex {
 public:
  constexpr VertData() {}
  constexpr VertData(float x_, float y_, float z_, uint8_t r_ = 255U,
                     uint8_t g_ = 255U, uint8_t b_ = 255U, uint8_t a_ = 255)
      : Vertex{x_, y_, z_, r_, g_, b_, a_} {}
  // copy constructor from base class
  constexpr VertData(const Vertex &other) : Vertex(other) {}

  explicit VertData(const Vector3 &xyz,
                    const Vector4 &rgba = Vector4{255, 255, 255, 255})
      : Vertex(xyz[0], xyz[1], xyz[2], static_cast<std::uint8_t>(rgba.x()),
               static_cast<std::uint8_t>(rgba.y()),
               static_cast<std::uint8_t>(rgba.z()),
               static_cast<std::uint8_t>(rgba.w())) {}
  operator Vector3() const { return Vector3{x, y, z}; }
  bool operator!=(const VertData &rhs) const {
    return x != rhs.x || y != rhs.y || z != rhs.z;
  }

  bool operator<(const VertData &rhs) const {
    if (x != rhs.x) {
      return x < rhs.x;
    } else if (y != rhs.y) {
      return y < rhs.y;
    } else if (z != rhs.z) {
      return z < rhs.z;
    } else {
      return false;
    }
  }
};
using FaceIndx = std::array<uint32_t, 3U>;
using EdgeIndx = std::array<uint32_t, 2U>;

class Pcl {
  std::string m_name{};
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

  explicit Pcl(const std::string &name) : m_name(name) {}

  // default copy constructor
  Pcl(const Pcl &other) = default;
  // default assignment operator
  Pcl &operator=(const Pcl &other) = default;
  // default move constructor
  Pcl(Pcl &&other) = default;
  // default move assignment operator
  Pcl &operator=(Pcl &&other) = default;
  // default destructor
  virtual ~Pcl() = default;

  const std::string &getName() const { return m_name; }
  void setName(const std::string &name) { m_name = name; }

  std::vector<VertData> &v() { return m_v; }
  const std::vector<VertData> &v() const { return m_v; }

  const Shader &shader() const;
  Shader &shader();
  Bbox3d getBbox() const;
  virtual std::optional<types::Vector3> get3dLocation(
      const std::uint32_t &prim_index,
      const std::array<types::Vector3, 2> &ray) const;

  bool &enabled() { return m_enabled; }
  bool enabled() const { return m_enabled; }
};

class Mesh : public Pcl {
 private:
  std::uint32_t m_ebo{0};
  std::vector<FaceIndx> m_f{};

 public:
  std::uint32_t &ebo() { return m_ebo; }
  std::uint32_t ebo() const { return m_ebo; }

  explicit Mesh(const std::string &name) : Pcl(name) {}

  // default copy constructor
  Mesh(const Mesh &other) = default;
  // default assignment operator
  Mesh &operator=(const Mesh &other) = default;
  // default move constructor
  Mesh(Mesh &&other) = default;
  // default move assignment operator
  Mesh &operator=(Mesh &&other) = default;
  // default destructor
  ~Mesh() override = default;

  std::vector<FaceIndx> &f() { return m_f; }
  const std::vector<FaceIndx> &f() const { return m_f; }
  std::optional<types::Vector3> get3dLocation(
      const std::uint32_t &prim_index,
      const std::array<types::Vector3, 2> &ray) const override;
};

class Edges : public Pcl {
 private:
  std::uint32_t m_ebo{0};
  std::vector<EdgeIndx> m_e{};

 public:
  std::uint32_t &ebo() { return m_ebo; }
  std::uint32_t ebo() const { return m_ebo; }

  explicit Edges(const std::string &name) : Pcl(name) {}

  // default copy constructor
  Edges(const Edges &other) = default;
  // default assignment operator
  Edges &operator=(const Edges &other) = default;
  // default move constructor
  Edges(Edges &&other) = default;
  // default move assignment operator
  Edges &operator=(Edges &&other) = default;
  // default destructor
  ~Edges() override = default;

  std::vector<EdgeIndx> &e() { return m_e; }
  const std::vector<EdgeIndx> &e() const { return m_e; }

  std::optional<types::Vector3> get3dLocation(
      const std::uint32_t &prim_index,
      const std::array<types::Vector3, 2> &ray) const override;
};
using Shape = std::variant<Pcl, Edges, Mesh>;

}  // namespace types
}  // namespace zview
