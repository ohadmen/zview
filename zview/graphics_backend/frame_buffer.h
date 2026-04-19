#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>

namespace zview {

class FrameBuffer {
 public:
  enum class TextureType : uint8_t { RGBA8, RGBA32UI };

  explicit FrameBuffer(TextureType textureType);
  ~FrameBuffer();
  FrameBuffer(const FrameBuffer&) = delete;
  FrameBuffer& operator=(const FrameBuffer&) = delete;
  FrameBuffer(FrameBuffer&&) = delete;
  FrameBuffer& operator=(FrameBuffer&&) = delete;

  bool resize(const std::array<int, 2>& wh);

  int width() const { return m_wh[0]; }
  int height() const { return m_wh[1]; }

  // Begin/end the render pass into this framebuffer.
  void bind(VkCommandBuffer cmd) const;
  void unbind(VkCommandBuffer cmd) const;

  VkFramebuffer framebuffer() const { return m_fb; }
  VkRenderPass renderPass() const { return m_renderPass; }
  VkImageView colorView() const { return m_colorView; }
  VkImage colorImage() const { return m_colorImage; }
  VkFormat colorFormat() const { return m_colorFormat; }

  // Returns the descriptor set for use as an ImGui texture (RGBA8 only).
  // Recreated on resize; valid until next resize() call.
  VkDescriptorSet imguiTexture() const { return m_imguiTex; }

 private:
  void destroyResources();
  bool createColorImage();
  bool createDepthImage();
  bool createFramebuffer();
  bool createImguiTex();

  TextureType m_type;
  VkFormat m_colorFormat{VK_FORMAT_UNDEFINED};
  VkFormat m_depthFormat{VK_FORMAT_D32_SFLOAT};

  VkImage m_colorImage{VK_NULL_HANDLE};
  VkImageView m_colorView{VK_NULL_HANDLE};
  VmaAllocation m_colorAlloc{VK_NULL_HANDLE};

  VkImage m_depthImage{VK_NULL_HANDLE};
  VkImageView m_depthView{VK_NULL_HANDLE};
  VmaAllocation m_depthAlloc{VK_NULL_HANDLE};

  VkRenderPass m_renderPass{VK_NULL_HANDLE};
  VkFramebuffer m_fb{VK_NULL_HANDLE};

  VkDescriptorSet m_imguiTex{VK_NULL_HANDLE};

  std::array<int, 2> m_wh{200, 200};
  bool m_initialized{false};
};

}  // namespace zview
