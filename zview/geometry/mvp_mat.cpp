
#include "zview/geometry/mvp_mat.h"

#include <imgui.h>  // for Imvec2

#include <limits>

#include "zview/params/params.h"

namespace zview {

bool qFuzzyIsNull(float d) {
  static constexpr float EPS = std::numeric_limits<float>::epsilon() * 1e3;
  return std::abs(d) < EPS;
}

MVPmat::MVPmat() { updatePmat(); }

types::Matrix4x4 MVPmat::getMVPmatrix() const {
  const auto mv = Eigen::Translation3f(0, 0, -m_viewDistance) * m_viewRotation *
                  m_modelTranslation;
  return m_proj.matrix() * mv.matrix();
}
void MVPmat::updatePmat() {
  const float aspect = getAspect();

  const float h_angle_rad = zview::Params::i().camera_fov_rad / 2.0f;
  const float nearPlane = m_viewDistance * Params::i().zmin_factor;
  const float farPlane = m_viewDistance * Params::i().zmax_factor;

  const float cotan =
      std::abs(h_angle_rad) < std::numeric_limits<float>::epsilon() * 1e3
          ? 1e3f
          : 1.0f / tanf(h_angle_rad);

  // following
  // https://github.com/radekp/qt/blob/master/zview/gui/math3d/qmatrix4x4.cpp
  float clip = farPlane - nearPlane;
  m_proj << cotan / aspect, 0, 0, 0, 0, cotan, 0, 0, 0, 0,
      -(nearPlane + farPlane) / clip, -2.0f * nearPlane * farPlane / clip, 0, 0,
      -1, 0;
}
float MVPmat::getAspect() const { return m_wh.x / m_wh.y; }
void MVPmat::setWinSize(const ImVec2 &wh) {
  m_wh = wh;
  updatePmat();
}
ImVec2 MVPmat::getWinSize() const { return m_wh; }
const Eigen::Quaternionf &MVPmat::getViewRotation() const {
  return m_viewRotation;
}

void MVPmat::setViewRotation(const Eigen::Quaternionf &m) {
  m_viewRotation = m;
}

float MVPmat::getViewDistance() const { return m_viewDistance; }

void MVPmat::setViewDistance(float m) {
  m_viewDistance = m;
  updatePmat();
}

const Eigen::Translation3f &MVPmat::getModelTranslation() const {
  return m_modelTranslation;
}

void MVPmat::setModelTranslation(const Eigen::Translation3f &m) {
  m_modelTranslation = m;
}

template <>
std::array<types::Vector3, 2> MVPmat::getRay(const ImVec2 &pt,
                                             CoordinateSystem s) const {
  std::array<types::Vector3, 2> cam_origin_ray;
  switch (s) {
    case MVPmat::CoordinateSystem::GLOBAL: {
      cam_origin_ray = getRay(pt, MVPmat::CoordinateSystem::CAMERA);
      cam_origin_ray[0] = m_modelTranslation.inverse() * cam_origin_ray[0];
      break;
    }
    case MVPmat::CoordinateSystem::CAMERA: {
      cam_origin_ray = getRay(pt, CoordinateSystem::SCREEN);
      cam_origin_ray[1] = m_viewRotation.inverse() * cam_origin_ray[1];
      cam_origin_ray[0] = m_viewRotation.inverse() *
                          Eigen::Translation3f(0, 0, m_viewDistance) *
                          cam_origin_ray[0];
      break;
    }
    case MVPmat::CoordinateSystem::SCREEN:
    default: {
      types::Vector4 uv(pt.x / m_wh.x * 2 - 1, -(pt.y / m_wh.y * 2 - 1), 1,
                        1.0f);
      types::Vector4 sv = m_proj.inverse() * uv;
      cam_origin_ray[0] = types::Vector3(0, 0, 0);
      cam_origin_ray[1] = types::Vector3(sv[0], sv[1], sv[2]).normalized();

      break;
    }
  }
  return cam_origin_ray;
}

types::Matrix4x4 MVPmat::getProjectiveMatrix() const { return m_proj; }

}  // namespace zview
