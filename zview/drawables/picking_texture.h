#pragma once
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>

#include "zview/graphics_backend/frame_buffer.h"
#include "zview/graphics_backend/shader.h"
#include "zview/types/types.h"

namespace zview {
class PickingTexture : public FrameBuffer {
 public:
  PickingTexture();
  ~PickingTexture();

  bool resize(const std::array<int, 2>& wh);
  void bind(VkCommandBuffer cmd);
  void unbind(VkCommandBuffer cmd);

  struct PixelInfo {
    std::uint32_t valid{0};
    std::uint32_t object_id{0};
    std::uint32_t prim_id{0};
  };

  PixelInfo readPixel(int x, int y) const;

  void drawShapeForPicking(VkCommandBuffer cmd, const types::Shape& shape,
                           std::uint32_t objectIndex,
                           const types::Matrix4x4& mvp);

 private:
  void drawPicking(VkCommandBuffer cmd, VkPipeline pipeline,
                   VkPipelineLayout layout, const PickingPushConstants& pc,
                   VkBuffer vbuf, uint32_t vtxCount, VkBuffer ibuf,
                   uint32_t idxCount);

  Shader m_pickingShader;    // POINT_LIST
  Shader m_meshPickShader;   // TRIANGLE_LIST
  Shader m_edgesPickShader;  // LINE_LIST
  VkBuffer m_readbackBuf{VK_NULL_HANDLE};
  VmaAllocation m_readbackAlloc{VK_NULL_HANDLE};
  int m_readbackW{0};
  int m_readbackH{0};
};
}  // namespace zview
