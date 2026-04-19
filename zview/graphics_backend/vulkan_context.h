#pragma once

#include <cstdio>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vk_mem_alloc.h>

#include <array>
#include <cstdint>
#include <functional>
#include <vector>

namespace zview {

static constexpr int kMaxFramesInFlight = 2;

struct VulkanContext {
  VkInstance instance{VK_NULL_HANDLE};
  VkPhysicalDevice physDevice{VK_NULL_HANDLE};
  VkDevice device{VK_NULL_HANDLE};
  VkSurfaceKHR surface{VK_NULL_HANDLE};
  VkQueue graphicsQueue{VK_NULL_HANDLE};
  VkQueue presentQueue{VK_NULL_HANDLE};
  uint32_t graphicsFamily{0};
  uint32_t presentFamily{0};
  VmaAllocator allocator{VK_NULL_HANDLE};

  VkSwapchainKHR swapchain{VK_NULL_HANDLE};
  VkFormat swapchainFormat{VK_FORMAT_UNDEFINED};
  VkExtent2D swapchainExtent{};
  std::vector<VkImage> swapImages;
  std::vector<VkImageView> swapViews;
  std::vector<VkFramebuffer> swapFramebuffers;

  VkRenderPass swapRenderPass{VK_NULL_HANDLE};
  VkRenderPass offscreenRenderPass{VK_NULL_HANDLE};
  VkRenderPass pickingRenderPass{VK_NULL_HANDLE};

  VkCommandPool cmdPool{VK_NULL_HANDLE};
  std::array<VkCommandBuffer, kMaxFramesInFlight> cmdBuffers{};
  std::array<VkSemaphore, kMaxFramesInFlight> imgAvailable{};
  std::array<VkSemaphore, kMaxFramesInFlight> renderDone{};
  std::array<VkFence, kMaxFramesInFlight> inFlight{};
  uint32_t currentFrame{0};
  uint32_t imageIndex{0};

  VkDescriptorPool imguiDescPool{VK_NULL_HANDLE};
  VkSampler defaultSampler{VK_NULL_HANDLE};

  static VulkanContext& get();

  bool init(GLFWwindow* window);
  void destroy();

  bool rebuildSwapchain(GLFWwindow* window);
  bool createOffscreenRenderPasses();

  VkCommandBuffer beginOneShot() const;
  void endOneShot(VkCommandBuffer cmd) const;

  void imageBarrier(
      VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout,
      VkImageLayout newLayout, VkAccessFlags srcAccess, VkAccessFlags dstAccess,
      VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage,
      VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT) const;

  static constexpr VkFormat kOffscreenColorFormat = VK_FORMAT_R8G8B8A8_UNORM;
  static constexpr VkFormat kPickingColorFormat = VK_FORMAT_R32G32B32A32_UINT;
  static constexpr VkFormat kDepthFormat = VK_FORMAT_D32_SFLOAT;

 private:
  VulkanContext() = default;
  bool createInstance();
  bool createSurface(GLFWwindow* window);
  bool pickPhysicalDevice();
  bool createLogicalDevice();
  bool createAllocator();
  bool createSwapchain(GLFWwindow* window);
  bool createSwapViews();
  bool createSwapRenderPass();
  bool createSwapFramebuffers();
  bool createCommandPool();
  bool createCommandBuffers();
  bool createSyncObjects();
  bool createImguiDescPool();
  bool createDefaultSampler();
};

inline VkResult vkCheck(VkResult r, const char* msg) {
  if (r != VK_SUCCESS) {
    fprintf(stderr, "Vulkan error %d: %s\n", (int)r, msg);
  }
  return r;
}

}  // namespace zview
