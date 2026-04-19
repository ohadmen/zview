#pragma once
#include <vulkan/vulkan.h>

#include "zview/types/types.h"
namespace zview {

struct ShapeDrawVisitor {
  void operator()(const types::Pcl &obj, VkCommandBuffer cmd,
                  const float *tform) const;
  void operator()(const types::Edges &obj, VkCommandBuffer cmd,
                  const float *tform) const;
  void operator()(const types::Mesh &obj, VkCommandBuffer cmd,
                  const float *tform) const;
};
}  // namespace zview
