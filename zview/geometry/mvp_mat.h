#pragma once
#include <imgui.h>

#include "zview/types/types.h"

namespace zview {

/**
 * @brief Class to handle the Model View Projection matrix
 *
 *
 */
class MVPmat {
  types::Matrix4x4 m_proj;
  Eigen::Quaternionf m_viewRotation{
      Eigen::AngleAxisf{std::acos(0.0f), types::Vector3{0, 0, 1}}};
  Eigen::Translation3f m_modelTranslation{0, 0, 0};
  ImVec2 m_wh{1, 1};
  float m_viewDistance{1.0f};

 public:
  MVPmat();

  types::Matrix4x4 getMVPmatrix() const;

  void setWinSize(const ImVec2 &wh);
  ImVec2 getWinSize() const;
  void updatePmat();
  float getAspect() const;

  enum class CoordinateSystem : std::uint8_t {
    GLOBAL = 0,
    CAMERA = 1,
    SCREEN = 2
  };

  // @brief Get the ray from the origin to the point in the specified coordinate
  // system
  // @tparam Pt the type of the point
  // @param pt the point
  // @param s the coordinate system
  // @return the ray as a start point and a direction vector
  template <typename Pt>
  std::array<types::Vector3, 2> getRay(const Pt &pt, CoordinateSystem s) const;

  const Eigen::Quaternionf &getViewRotation() const;

  void setViewRotation(const Eigen::Quaternionf &m);

  float getViewDistance() const;

  void setViewDistance(float m);

  const Eigen::Translation3f &getModelTranslation() const;

  void setModelTranslation(const Eigen::Translation3f &m);

  types::Matrix4x4 getProjectiveMatrix() const;
};

}  // namespace zview
