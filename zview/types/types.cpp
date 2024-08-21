#include "zview/types/types.h"

#include <limits>
#include <string>
namespace zview {
namespace types {
Bbox3d Pcl::getBbox() const {
  static const float e = 0.001f;
  if (m_v.size() == 0) {
    return {{-e, -e, -e}, {e, e, e}};
  }
  Vector3 mmin(std::numeric_limits<float>::max(),
               std::numeric_limits<float>::max(),
               std::numeric_limits<float>::max());
  Vector3 mmax(std::numeric_limits<float>::lowest(),
               std::numeric_limits<float>::lowest(),
               std::numeric_limits<float>::lowest());
  for (const auto &a : m_v) {
    mmin = mmin.cwiseMin(Vector3{a.x, a.y, a.z});
    mmax = mmax.cwiseMax(Vector3{a.x, a.y, a.z});
  }
  return {mmin, mmax};
}
std::array<types::Vector3, 8> Bbox3d::getVertices() const {
  return {Vector3{min().x(), min().y(), min().z()},
          Vector3{min().x(), min().y(), max().z()},
          Vector3{min().x(), max().y(), min().z()},
          Vector3{min().x(), max().y(), max().z()},
          Vector3{max().x(), min().y(), min().z()},
          Vector3{max().x(), min().y(), max().z()},
          Vector3{max().x(), max().y(), min().z()},
          Vector3{max().x(), max().y(), max().z()}};
}

std::optional<types::Vector3> Pcl::get3dLocation(
    const std::uint32_t &prim_index,
    [[maybe_unused]] const std::array<types::Vector3, 2> &ray) const {
  if (prim_index >= m_v.size()) {
    return std::nullopt;
  }
  return Vector3{m_v[prim_index].x, m_v[prim_index].y, m_v[prim_index].z};
}

std::optional<types::Vector3> Mesh::get3dLocation(
    const std::uint32_t &prim_index,
    const std::array<types::Vector3, 2> &ray) const {
  if (prim_index >= v().size()) {
    return std::nullopt;
  }
  // plane represent as n'*x=d
  // ray represented as x(t)=p+m*t
  // n,x,p,m are R3, t,d are R1
  // solve: n'(p+m*t)=d
  // t=(d-n'd)/n'm
  // assuming that the ray actually hits the said triangle (as it comes from the
  // picking shader)
  const auto ind = m_f[prim_index];
  const types::Vector3 v0{v()[ind[0]]};
  const types::Vector3 v1{v()[ind[1]]};
  const types::Vector3 v2{v()[ind[2]]};

  // intersection between ray and triangle
  const auto u = v1 - v0;
  const auto v = v2 - v0;
  const auto n = u.cross(v).normalized();
  if (n.squaredNorm() == 0) {  // triangle is degenerate
    return std::nullopt;       // do not deal with this case
  }

  float b = n.dot(ray[1]);
  if (b == 0) {  // ray is  parallel to triangle plane
    return std::nullopt;
  }
  float a = n.dot(v0 - ray[0]);
  // get intersect point of ray with triangle plane
  float r = a / b;
  if (r < 0.0) {          // ray goes away from triangle
    return std::nullopt;  // => no intersect
  }

  auto pt = ray[0] + ray[1] * r;  // intersect point of ray and plane
  return pt;
}

std::optional<types::Vector3> Edges::get3dLocation(
    const std::uint32_t &prim_index,
    const std::array<types::Vector3, 2> &ray) const {
  if (prim_index >= v().size()) {
    return std::nullopt;
  }

  const auto v0 = types::Vector3{v()[m_e[prim_index][0]]};
  const auto v1 = types::Vector3{v()[m_e[prim_index][1]]};

  // intersection between ray and a segment [v0,v1]
  const auto u0 = (v1 - v0).normalized();
  const auto &u1 = ray[1];

  const auto &p0 = v0;
  const auto &p1 = ray[0];
  if (u0.squaredNorm() == 0)  // segment is degenerate
    return std::nullopt;      // do not deal with this case

  Eigen ::Matrix<float, 3, 2> matA;
  matA << u0, -u1;
  Eigen::Matrix<float, 3, 1> matB = p1 - p0;
  const Eigen ::Matrix<float, 2, 3> matApinv =
      matA.completeOrthogonalDecomposition().pseudoInverse();
  const Eigen ::Matrix<float, 2, 1> x = matApinv * matB;
  auto res = (p0 + p1 + x[0] * u0 + x[1] * u1) / 2;
  return res;
}

const Shader &Pcl::shader() const { return m_shader; }
Shader &Pcl::shader() { return m_shader; }

}  // namespace types
}  // namespace zview
