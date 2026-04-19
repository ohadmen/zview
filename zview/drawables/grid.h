#pragma once
#include <vulkan/vulkan.h>

#include "zview/types/types.h"
namespace zview {
class Grid {
  types::Edges m_minor;
  Shader m_shader;

 public:
  Grid();
  bool init();
  void draw(VkCommandBuffer cmd, const types::Matrix4x4& mvp,
            const types::Vector3& model_loc, const float d) const;
};
}  // namespace zview
