#define VMA_IMPLEMENTATION
#include "zview/graphics_backend/vulkan_context.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <set>
#include <vector>

namespace zview {

VulkanContext& VulkanContext::get() {
  static VulkanContext ctx;
  return ctx;
}

bool VulkanContext::init(GLFWwindow* window) {
  return createInstance() && createSurface(window) && pickPhysicalDevice() &&
         createLogicalDevice() && createAllocator() &&
         createSwapchain(window) && createSwapViews() &&
         createSwapRenderPass() && createOffscreenRenderPasses() &&
         createSwapFramebuffers() && createCommandPool() &&
         createCommandBuffers() && createSyncObjects() &&
         createImguiDescPool() && createDefaultSampler();
}

// ---- Instance ---------------------------------------------------------------

bool VulkanContext::createInstance() {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "zview";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_2;

  uint32_t glfwExtCount = 0;
  const char** glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);
  std::vector<const char*> exts(glfwExts, glfwExts + glfwExtCount);

  VkInstanceCreateInfo ci{};
  ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  ci.pApplicationInfo = &appInfo;
  ci.enabledExtensionCount = (uint32_t)exts.size();
  ci.ppEnabledExtensionNames = exts.data();
  ci.enabledLayerCount = 0;

#ifndef NDEBUG
  const char* layer = "VK_LAYER_KHRONOS_validation";
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> layers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
  bool found = false;
  for (auto& l : layers) {
    if (strcmp(l.layerName, layer) == 0) {
      found = true;
      break;
    }
  }
  if (found) {
    ci.enabledLayerCount = 1;
    ci.ppEnabledLayerNames = &layer;
  }
#endif

  return vkCheck(vkCreateInstance(&ci, nullptr, &instance),
                 "vkCreateInstance") == VK_SUCCESS;
}

// ---- Surface ----------------------------------------------------------------

bool VulkanContext::createSurface(GLFWwindow* window) {
  return vkCheck(glfwCreateWindowSurface(instance, window, nullptr, &surface),
                 "glfwCreateWindowSurface") == VK_SUCCESS;
}

// ---- Physical device --------------------------------------------------------

static bool deviceSuitable(VkPhysicalDevice dev, VkSurfaceKHR surf,
                           uint32_t& gfx, uint32_t& pres) {
  uint32_t qc = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(dev, &qc, nullptr);
  std::vector<VkQueueFamilyProperties> qprops(qc);
  vkGetPhysicalDeviceQueueFamilyProperties(dev, &qc, qprops.data());
  gfx = pres = UINT32_MAX;
  for (uint32_t i = 0; i < qc; ++i) {
    if (qprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) gfx = i;
    VkBool32 ps = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surf, &ps);
    if (ps) pres = i;
    if (gfx != UINT32_MAX && pres != UINT32_MAX) break;
  }
  if (gfx == UINT32_MAX || pres == UINT32_MAX) return false;
  uint32_t ec = 0;
  vkEnumerateDeviceExtensionProperties(dev, nullptr, &ec, nullptr);
  std::vector<VkExtensionProperties> exts(ec);
  vkEnumerateDeviceExtensionProperties(dev, nullptr, &ec, exts.data());
  for (auto& e : exts) {
    if (strcmp(e.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
      return true;
  }
  return false;
}

bool VulkanContext::pickPhysicalDevice() {
  uint32_t count = 0;
  vkEnumeratePhysicalDevices(instance, &count, nullptr);
  if (count == 0) {
    std::cerr << "No Vulkan devices found\n";
    return false;
  }
  std::vector<VkPhysicalDevice> devs(count);
  vkEnumeratePhysicalDevices(instance, &count, devs.data());
  for (auto* dev : devs) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(dev, &props);
    uint32_t g, p;
    if (!deviceSuitable(dev, surface, g, p)) continue;
    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      physDevice = dev;
      graphicsFamily = g;
      presentFamily = p;
      return true;
    }
  }
  uint32_t g, p;
  for (auto* dev : devs) {
    if (deviceSuitable(dev, surface, g, p)) {
      physDevice = dev;
      graphicsFamily = g;
      presentFamily = p;
      return true;
    }
  }
  std::cerr << "No suitable Vulkan device\n";
  return false;
}

// ---- Logical device ---------------------------------------------------------

bool VulkanContext::createLogicalDevice() {
  std::set<uint32_t> families = {graphicsFamily, presentFamily};
  std::vector<VkDeviceQueueCreateInfo> qcis;
  float pri = 1.0f;
  for (uint32_t f : families) {
    VkDeviceQueueCreateInfo qi{};
    qi.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qi.queueFamilyIndex = f;
    qi.queueCount = 1;
    qi.pQueuePriorities = &pri;
    qcis.push_back(qi);
  }
  VkPhysicalDeviceFeatures features{};
  features.largePoints = VK_TRUE;
  features.fillModeNonSolid = VK_TRUE;

  const char* swapExt = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
  VkDeviceCreateInfo ci{};
  ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  ci.queueCreateInfoCount = (uint32_t)qcis.size();
  ci.pQueueCreateInfos = qcis.data();
  ci.enabledExtensionCount = 1;
  ci.ppEnabledExtensionNames = &swapExt;
  ci.pEnabledFeatures = &features;

  if (vkCheck(vkCreateDevice(physDevice, &ci, nullptr, &device),
              "vkCreateDevice") != VK_SUCCESS)
    return false;
  vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
  vkGetDeviceQueue(device, presentFamily, 0, &presentQueue);
  return true;
}

// ---- VMA allocator ----------------------------------------------------------

bool VulkanContext::createAllocator() {
  VmaVulkanFunctions vf{};
  vf.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
  vf.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
  VmaAllocatorCreateInfo ai{};
  ai.instance = instance;
  ai.physicalDevice = physDevice;
  ai.device = device;
  ai.vulkanApiVersion = VK_API_VERSION_1_2;
  ai.pVulkanFunctions = &vf;
  return vkCheck(vmaCreateAllocator(&ai, &allocator), "vmaCreateAllocator") ==
         VK_SUCCESS;
}

// ---- Swapchain --------------------------------------------------------------

bool VulkanContext::createSwapchain(GLFWwindow* window) {
  VkSurfaceCapabilitiesKHR caps;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &caps);

  uint32_t fc = 0, pc = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &fc, nullptr);
  std::vector<VkSurfaceFormatKHR> formats(fc);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &fc,
                                       formats.data());
  vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &pc, nullptr);
  std::vector<VkPresentModeKHR> pmodes(pc);
  vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &pc,
                                            pmodes.data());

  VkSurfaceFormatKHR fmt = formats[0];
  for (auto& f : formats)
    if (f.format == VK_FORMAT_B8G8R8A8_SRGB &&
        f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      fmt = f;
      break;
    }
  VkPresentModeKHR pm = VK_PRESENT_MODE_FIFO_KHR;
  for (auto& m : pmodes)
    if (m == VK_PRESENT_MODE_MAILBOX_KHR) {
      pm = m;
      break;
    }

  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  VkExtent2D ext{(uint32_t)w, (uint32_t)h};
  ext.width = std::clamp(ext.width, caps.minImageExtent.width,
                         caps.maxImageExtent.width);
  ext.height = std::clamp(ext.height, caps.minImageExtent.height,
                          caps.maxImageExtent.height);

  uint32_t imgCount = caps.minImageCount + 1;
  if (caps.maxImageCount > 0) imgCount = std::min(imgCount, caps.maxImageCount);

  uint32_t qfi[] = {graphicsFamily, presentFamily};
  VkSwapchainCreateInfoKHR ci{};
  ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  ci.surface = surface;
  ci.minImageCount = imgCount;
  ci.imageFormat = fmt.format;
  ci.imageColorSpace = fmt.colorSpace;
  ci.imageExtent = ext;
  ci.imageArrayLayers = 1;
  ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  if (graphicsFamily != presentFamily) {
    ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    ci.queueFamilyIndexCount = 2;
    ci.pQueueFamilyIndices = qfi;
  } else {
    ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }
  ci.preTransform = caps.currentTransform;
  ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  ci.presentMode = pm;
  ci.clipped = VK_TRUE;

  if (vkCheck(vkCreateSwapchainKHR(device, &ci, nullptr, &swapchain),
              "vkCreateSwapchainKHR") != VK_SUCCESS)
    return false;
  swapchainFormat = fmt.format;
  swapchainExtent = ext;
  uint32_t cnt = 0;
  vkGetSwapchainImagesKHR(device, swapchain, &cnt, nullptr);
  swapImages.resize(cnt);
  vkGetSwapchainImagesKHR(device, swapchain, &cnt, swapImages.data());
  return true;
}

bool VulkanContext::createSwapViews() {
  swapViews.resize(swapImages.size());
  for (size_t i = 0; i < swapImages.size(); ++i) {
    VkImageViewCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ci.image = swapImages[i];
    ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ci.format = swapchainFormat;
    ci.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    if (vkCheck(vkCreateImageView(device, &ci, nullptr, &swapViews[i]),
                "vkCreateImageView") != VK_SUCCESS)
      return false;
  }
  return true;
}

bool VulkanContext::createSwapRenderPass() {
  VkAttachmentDescription color{};
  color.format = swapchainFormat;
  color.samples = VK_SAMPLE_COUNT_1_BIT;
  color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorRef{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
  VkSubpassDescription sub{};
  sub.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  sub.colorAttachmentCount = 1;
  sub.pColorAttachments = &colorRef;

  VkSubpassDependency dep{};
  dep.srcSubpass = VK_SUBPASS_EXTERNAL;
  dep.dstSubpass = 0;
  dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.srcAccessMask = 0;
  dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo rpci{};
  rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  rpci.attachmentCount = 1;
  rpci.pAttachments = &color;
  rpci.subpassCount = 1;
  rpci.pSubpasses = &sub;
  rpci.dependencyCount = 1;
  rpci.pDependencies = &dep;
  return vkCheck(vkCreateRenderPass(device, &rpci, nullptr, &swapRenderPass),
                 "vkCreateRenderPass(swap)") == VK_SUCCESS;
}

bool VulkanContext::createSwapFramebuffers() {
  swapFramebuffers.resize(swapViews.size());
  for (size_t i = 0; i < swapViews.size(); ++i) {
    VkFramebufferCreateInfo fci{};
    fci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fci.renderPass = swapRenderPass;
    fci.attachmentCount = 1;
    fci.pAttachments = &swapViews[i];
    fci.width = swapchainExtent.width;
    fci.height = swapchainExtent.height;
    fci.layers = 1;
    if (vkCheck(
            vkCreateFramebuffer(device, &fci, nullptr, &swapFramebuffers[i]),
            "vkCreateFramebuffer(swap)") != VK_SUCCESS)
      return false;
  }
  return true;
}

bool VulkanContext::rebuildSwapchain(GLFWwindow* window) {
  vkDeviceWaitIdle(device);
  for (auto fb : swapFramebuffers) vkDestroyFramebuffer(device, fb, nullptr);
  for (auto iv : swapViews) vkDestroyImageView(device, iv, nullptr);
  vkDestroySwapchainKHR(device, swapchain, nullptr);
  return createSwapchain(window) && createSwapViews() &&
         createSwapFramebuffers();
}

// ---- Offscreen render passes ------------------------------------------------

static bool makeOffscreenRP(VkDevice device, VkFormat colorFmt,
                            VkFormat depthFmt, VkImageLayout colorFinalLayout,
                            VkSampleCountFlagBits samples, VkRenderPass& rp) {
  const bool msaa = samples != VK_SAMPLE_COUNT_1_BIT;

  // Attachment 0: color (MSAA when msaa, otherwise single-sample final)
  VkAttachmentDescription color{};
  color.format = colorFmt;
  color.samples = samples;
  color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color.storeOp =
      msaa ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;
  color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color.finalLayout =
      msaa ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : colorFinalLayout;

  // Attachment 1: depth (multisampled when msaa)
  VkAttachmentDescription depth{};
  depth.format = depthFmt;
  depth.samples = samples;
  depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorRef{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
  VkAttachmentReference depthRef{
      1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

  VkSubpassDescription sub{};
  sub.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  sub.colorAttachmentCount = 1;
  sub.pColorAttachments = &colorRef;
  sub.pDepthStencilAttachment = &depthRef;

  // Entry: wait for previous frame's reads before writing attachments
  VkSubpassDependency deps[2]{};
  deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  deps[0].dstSubpass = 0;
  deps[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                         VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  deps[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                         VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  deps[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  // Exit: ensure color writes (including MSAA resolve) are visible to
  // subsequent fragment reads
  deps[1].srcSubpass = 0;
  deps[1].dstSubpass = VK_SUBPASS_EXTERNAL;
  deps[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  deps[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  deps[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  deps[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

  if (!msaa) {
    VkAttachmentDescription atts[] = {color, depth};
    VkRenderPassCreateInfo rpci{};
    rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpci.attachmentCount = 2;
    rpci.pAttachments = atts;
    rpci.subpassCount = 1;
    rpci.pSubpasses = &sub;
    rpci.dependencyCount = 2;
    rpci.pDependencies = deps;
    return vkCheck(vkCreateRenderPass(device, &rpci, nullptr, &rp),
                   "vkCreateRenderPass(offscreen)") == VK_SUCCESS;
  }

  // Attachment 2: resolve (single-sample, written by subpass resolve)
  VkAttachmentDescription resolve{};
  resolve.format = colorFmt;
  resolve.samples = VK_SAMPLE_COUNT_1_BIT;
  resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  resolve.finalLayout = colorFinalLayout;

  VkAttachmentReference resolveRef{2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
  sub.pResolveAttachments = &resolveRef;

  VkAttachmentDescription atts[] = {color, depth, resolve};
  VkRenderPassCreateInfo rpci{};
  rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  rpci.attachmentCount = 3;
  rpci.pAttachments = atts;
  rpci.subpassCount = 1;
  rpci.pSubpasses = &sub;
  rpci.dependencyCount = 2;
  rpci.pDependencies = deps;
  return vkCheck(vkCreateRenderPass(device, &rpci, nullptr, &rp),
                 "vkCreateRenderPass(offscreen-msaa)") == VK_SUCCESS;
}

bool VulkanContext::createOffscreenRenderPasses() {
  return makeOffscreenRP(device, kOffscreenColorFormat, kDepthFormat,
                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                         VK_SAMPLE_COUNT_1_BIT, offscreenRenderPass) &&
         makeOffscreenRP(device, kPickingColorFormat, kDepthFormat,
                         VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                         VK_SAMPLE_COUNT_1_BIT, pickingRenderPass);
}

// ---- Command pool / buffers -------------------------------------------------

bool VulkanContext::createCommandPool() {
  VkCommandPoolCreateInfo ci{};
  ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  ci.queueFamilyIndex = graphicsFamily;
  ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  return vkCheck(vkCreateCommandPool(device, &ci, nullptr, &cmdPool),
                 "vkCreateCommandPool") == VK_SUCCESS;
}

bool VulkanContext::createCommandBuffers() {
  VkCommandBufferAllocateInfo ai{};
  ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  ai.commandPool = cmdPool;
  ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  ai.commandBufferCount = kMaxFramesInFlight;
  return vkCheck(vkAllocateCommandBuffers(device, &ai, cmdBuffers.data()),
                 "vkAllocateCommandBuffers") == VK_SUCCESS;
}

// ---- Sync objects -----------------------------------------------------------

bool VulkanContext::createSyncObjects() {
  VkSemaphoreCreateInfo si{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  VkFenceCreateInfo fi{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
  fi.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  for (int i = 0; i < kMaxFramesInFlight; ++i) {
    if (vkCheck(vkCreateSemaphore(device, &si, nullptr, &imgAvailable[i]),
                "sem imgAvailable") != VK_SUCCESS)
      return false;
    if (vkCheck(vkCreateSemaphore(device, &si, nullptr, &renderDone[i]),
                "sem renderDone") != VK_SUCCESS)
      return false;
    if (vkCheck(vkCreateFence(device, &fi, nullptr, &inFlight[i]),
                "fence inFlight") != VK_SUCCESS)
      return false;
  }
  return true;
}

// ---- ImGui descriptor pool --------------------------------------------------

bool VulkanContext::createImguiDescPool() {
  VkDescriptorPoolSize sizes[] = {
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
  };
  VkDescriptorPoolCreateInfo ci{};
  ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  ci.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  ci.maxSets = 1000;
  ci.poolSizeCount = 2;
  ci.pPoolSizes = sizes;
  return vkCheck(vkCreateDescriptorPool(device, &ci, nullptr, &imguiDescPool),
                 "vkCreateDescriptorPool(imgui)") == VK_SUCCESS;
}

bool VulkanContext::createDefaultSampler() {
  VkSamplerCreateInfo si{};
  si.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  si.magFilter = VK_FILTER_LINEAR;
  si.minFilter = VK_FILTER_LINEAR;
  si.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  si.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  si.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  return vkCheck(vkCreateSampler(device, &si, nullptr, &defaultSampler),
                 "vkCreateSampler") == VK_SUCCESS;
}

// ---- One-shot command buffer ------------------------------------------------

VkCommandBuffer VulkanContext::beginOneShot() const {
  VkCommandBufferAllocateInfo ai{};
  ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  ai.commandPool = cmdPool;
  ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  ai.commandBufferCount = 1;
  VkCommandBuffer cmd;
  vkAllocateCommandBuffers(device, &ai, &cmd);
  VkCommandBufferBeginInfo bi{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(cmd, &bi);
  return cmd;
}

void VulkanContext::endOneShot(VkCommandBuffer cmd) const {
  vkEndCommandBuffer(cmd);
  VkSubmitInfo si{VK_STRUCTURE_TYPE_SUBMIT_INFO};
  si.commandBufferCount = 1;
  si.pCommandBuffers = &cmd;
  VkFenceCreateInfo fi{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
  VkFence fence;
  vkCreateFence(device, &fi, nullptr, &fence);
  vkQueueSubmit(graphicsQueue, 1, &si, fence);
  vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
  vkDestroyFence(device, fence, nullptr);
  vkFreeCommandBuffers(device, cmdPool, 1, &cmd);
}

// ---- Image barrier ----------------------------------------------------------

void VulkanContext::imageBarrier(
    VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout,
    VkImageLayout newLayout, VkAccessFlags srcAccess, VkAccessFlags dstAccess,
    VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage,
    VkImageAspectFlags aspect) const {
  VkImageMemoryBarrier b{};
  b.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  b.oldLayout = oldLayout;
  b.newLayout = newLayout;
  b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  b.image = image;
  b.subresourceRange = {aspect, 0, 1, 0, 1};
  b.srcAccessMask = srcAccess;
  b.dstAccessMask = dstAccess;
  vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1,
                       &b);
}

// ---- Destroy ----------------------------------------------------------------

void VulkanContext::destroy() {
  vkDeviceWaitIdle(device);
  if (defaultSampler) vkDestroySampler(device, defaultSampler, nullptr);
  if (imguiDescPool) vkDestroyDescriptorPool(device, imguiDescPool, nullptr);
  for (int i = 0; i < kMaxFramesInFlight; ++i) {
    if (inFlight[i]) vkDestroyFence(device, inFlight[i], nullptr);
    if (renderDone[i]) vkDestroySemaphore(device, renderDone[i], nullptr);
    if (imgAvailable[i]) vkDestroySemaphore(device, imgAvailable[i], nullptr);
  }
  if (cmdPool) vkDestroyCommandPool(device, cmdPool, nullptr);
  for (auto fb : swapFramebuffers) vkDestroyFramebuffer(device, fb, nullptr);
  if (pickingRenderPass)
    vkDestroyRenderPass(device, pickingRenderPass, nullptr);
  if (offscreenRenderPass)
    vkDestroyRenderPass(device, offscreenRenderPass, nullptr);
  if (swapRenderPass) vkDestroyRenderPass(device, swapRenderPass, nullptr);
  for (auto iv : swapViews) vkDestroyImageView(device, iv, nullptr);
  if (swapchain) vkDestroySwapchainKHR(device, swapchain, nullptr);
  if (allocator) vmaDestroyAllocator(allocator);
  if (device) vkDestroyDevice(device, nullptr);
  if (surface) vkDestroySurfaceKHR(instance, surface, nullptr);
  if (instance) vkDestroyInstance(instance, nullptr);
}

}  // namespace zview
