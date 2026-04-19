#include "zview/graphics_backend/frame_buffer.h"

#include "imgui_impl_vulkan.h"
#include "zview/graphics_backend/vulkan_context.h"

namespace zview {

FrameBuffer::FrameBuffer(TextureType textureType) : m_type(textureType) {}

FrameBuffer::~FrameBuffer() { destroyResources(); }

void FrameBuffer::destroyResources() {
  auto& ctx = VulkanContext::get();
  if (ctx.device == VK_NULL_HANDLE) return;
  vkDeviceWaitIdle(ctx.device);

  if (m_imguiTex != VK_NULL_HANDLE) {
    ImGui_ImplVulkan_RemoveTexture(m_imguiTex);
    m_imguiTex = VK_NULL_HANDLE;
  }
  if (m_fb) {
    vkDestroyFramebuffer(ctx.device, m_fb, nullptr);
    m_fb = VK_NULL_HANDLE;
  }
  if (m_colorView) {
    vkDestroyImageView(ctx.device, m_colorView, nullptr);
    m_colorView = VK_NULL_HANDLE;
  }
  if (m_depthView) {
    vkDestroyImageView(ctx.device, m_depthView, nullptr);
    m_depthView = VK_NULL_HANDLE;
  }
  if (m_colorImage) {
    vmaDestroyImage(ctx.allocator, m_colorImage, m_colorAlloc);
    m_colorImage = VK_NULL_HANDLE;
  }
  if (m_depthImage) {
    vmaDestroyImage(ctx.allocator, m_depthImage, m_depthAlloc);
    m_depthImage = VK_NULL_HANDLE;
  }
  m_initialized = false;
}

bool FrameBuffer::resize(const std::array<int, 2>& wh) {
  if (wh == m_wh && m_initialized) return true;
  m_wh = wh;
  if (m_colorFormat == VK_FORMAT_UNDEFINED) {
    auto& ctx = VulkanContext::get();
    switch (m_type) {
      case TextureType::RGBA8:
        m_colorFormat = ctx.kOffscreenColorFormat;
        m_renderPass = ctx.offscreenRenderPass;
        break;
      case TextureType::RGBA32UI:
        m_colorFormat = ctx.kPickingColorFormat;
        m_renderPass = ctx.pickingRenderPass;
        break;
    }
    m_depthFormat = ctx.kDepthFormat;
  }
  destroyResources();
  if (!createColorImage()) return false;
  if (!createDepthImage()) return false;
  if (!createFramebuffer()) return false;
  if (m_type == TextureType::RGBA8 && !createImguiTex()) return false;
  m_initialized = true;
  return true;
}

static bool makeImageAndView(VkFormat fmt, uint32_t w, uint32_t h,
                             VkImageUsageFlags usage, VkImageAspectFlags aspect,
                             VkImage& img, VkImageView& view,
                             VmaAllocation& alloc) {
  auto& ctx = VulkanContext::get();
  VkImageCreateInfo ici{};
  ici.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  ici.imageType = VK_IMAGE_TYPE_2D;
  ici.format = fmt;
  ici.extent = {w, h, 1};
  ici.mipLevels = 1;
  ici.arrayLayers = 1;
  ici.samples = VK_SAMPLE_COUNT_1_BIT;
  ici.tiling = VK_IMAGE_TILING_OPTIMAL;
  ici.usage = usage;
  ici.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  ici.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  VmaAllocationCreateInfo aci{};
  aci.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  if (vmaCreateImage(ctx.allocator, &ici, &aci, &img, &alloc, nullptr) !=
      VK_SUCCESS)
    return false;

  VkImageViewCreateInfo vci{};
  vci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  vci.image = img;
  vci.viewType = VK_IMAGE_VIEW_TYPE_2D;
  vci.format = fmt;
  vci.subresourceRange = {aspect, 0, 1, 0, 1};
  return vkCheck(vkCreateImageView(ctx.device, &vci, nullptr, &view),
                 "vkCreateImageView") == VK_SUCCESS;
}

bool FrameBuffer::createColorImage() {
  VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                            VK_IMAGE_USAGE_SAMPLED_BIT |
                            VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  return makeImageAndView(m_colorFormat, m_wh[0], m_wh[1], usage,
                          VK_IMAGE_ASPECT_COLOR_BIT, m_colorImage, m_colorView,
                          m_colorAlloc);
}

bool FrameBuffer::createDepthImage() {
  return makeImageAndView(m_depthFormat, m_wh[0], m_wh[1],
                          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                          VK_IMAGE_ASPECT_DEPTH_BIT, m_depthImage, m_depthView,
                          m_depthAlloc);
}

bool FrameBuffer::createFramebuffer() {
  auto& ctx = VulkanContext::get();
  VkImageView atts[] = {m_colorView, m_depthView};
  VkFramebufferCreateInfo fci{};
  fci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  fci.renderPass = m_renderPass;
  fci.attachmentCount = 2;
  fci.pAttachments = atts;
  fci.width = m_wh[0];
  fci.height = m_wh[1];
  fci.layers = 1;
  return vkCheck(vkCreateFramebuffer(ctx.device, &fci, nullptr, &m_fb),
                 "vkCreateFramebuffer") == VK_SUCCESS;
}

bool FrameBuffer::createImguiTex() {
  auto& ctx = VulkanContext::get();
  m_imguiTex =
      ImGui_ImplVulkan_AddTexture(ctx.defaultSampler, m_colorView,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  return m_imguiTex != VK_NULL_HANDLE;
}

void FrameBuffer::bind(VkCommandBuffer cmd) const {
  VkClearValue clears[2];
  clears[0].color = {{0.f, 0.f, 0.f, 1.f}};
  clears[1].depthStencil = {1.f, 0};
  VkRenderPassBeginInfo rbi{};
  rbi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  rbi.renderPass = m_renderPass;
  rbi.framebuffer = m_fb;
  rbi.renderArea.extent = {(uint32_t)m_wh[0], (uint32_t)m_wh[1]};
  rbi.clearValueCount = 2;
  rbi.pClearValues = clears;
  vkCmdBeginRenderPass(cmd, &rbi, VK_SUBPASS_CONTENTS_INLINE);
  VkViewport vp{0, 0, (float)m_wh[0], (float)m_wh[1], 0.f, 1.f};
  VkRect2D sc{{0, 0}, {(uint32_t)m_wh[0], (uint32_t)m_wh[1]}};
  vkCmdSetViewport(cmd, 0, 1, &vp);
  vkCmdSetScissor(cmd, 0, 1, &sc);
}

void FrameBuffer::unbind(VkCommandBuffer cmd) const { vkCmdEndRenderPass(cmd); }

}  // namespace zview
