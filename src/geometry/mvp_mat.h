#pragma once
#include "src/types/types.h"

namespace zview {

/**
 * @brief Class to handle the Model View Projection matrix
 *
 *
 */
class MVPmat {
  types::Matrix4x4 m_proj;
  Eigen::Quaternionf m_viewRotation;
  Eigen::Translation3f m_modelTranslation;
  int m_w;
  int m_h;
  float m_viewDistance;

 public:
  MVPmat();

  types::Matrix4x4 getMVPmatrix() const;

  void setWinSize(const std::array<int, 2> &wh);
  std::array<int, 2> getWinSize() const;
  void updatePmat();
  float getAspect() const;

  enum class CoordinateSystem : std::uint8_t {
    GLOBAL = 0,
    CAMERA = 1,
    SCREEN = 2
  };

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