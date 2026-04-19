#pragma once
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

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
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
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
    if (x != rhs.x) return x < rhs.x;
    if (y != rhs.y) return y < rhs.y;
    if (z != rhs.z) return z < rhs.z;
    return false;
  }
};
using FaceIndx = std::array<uint32_t, 3U>;
using EdgeIndx = std::array<uint32_t, 2U>;

class Pcl {
  std::string m_name{};
  std::vector<types::VertData> m_v{};
  Shader m_shader{};
  bool m_enabled{true};

 public:
  VkBuffer vertexBuffer{VK_NULL_HANDLE};
  VmaAllocation vertexAlloc{VK_NULL_HANDLE};

  explicit Pcl(const std::string &name) : m_name(name) {}

  Pcl(const Pcl &) = delete;
  Pcl &operator=(const Pcl &) = delete;
  Pcl(Pcl &&) = default;
  Pcl &operator=(Pcl &&) = default;
  virtual ~Pcl();

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
 public:
  VkBuffer indexBuffer{VK_NULL_HANDLE};
  VmaAllocation indexAlloc{VK_NULL_HANDLE};

 private:
  std::vector<FaceIndx> m_f{};

 public:
  explicit Mesh(const std::string &name) : Pcl(name) {}

  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;
  Mesh(Mesh &&) = default;
  Mesh &operator=(Mesh &&) = default;
  ~Mesh() override;

  std::vector<FaceIndx> &f() { return m_f; }
  const std::vector<FaceIndx> &f() const { return m_f; }
  std::optional<types::Vector3> get3dLocation(
      const std::uint32_t &prim_index,
      const std::array<types::Vector3, 2> &ray) const override;
};

class Edges : public Pcl {
 public:
  VkBuffer indexBuffer{VK_NULL_HANDLE};
  VmaAllocation indexAlloc{VK_NULL_HANDLE};

 private:
  std::vector<EdgeIndx> m_e{};

 public:
  explicit Edges(const std::string &name) : Pcl(name) {}

  Edges(const Edges &) = delete;
  Edges &operator=(const Edges &) = delete;
  Edges(Edges &&) = default;
  Edges &operator=(Edges &&) = default;
  ~Edges() override;

  std::vector<EdgeIndx> &e() { return m_e; }
  const std::vector<EdgeIndx> &e() const { return m_e; }

  std::optional<types::Vector3> get3dLocation(
      const std::uint32_t &prim_index,
      const std::array<types::Vector3, 2> &ray) const override;
};
using Shape = std::variant<Pcl, Edges, Mesh>;

}  // namespace types
}  // namespace zview
