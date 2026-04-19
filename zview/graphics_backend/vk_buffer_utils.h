#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <cstddef>
#include <cstring>

#include "zview/graphics_backend/vulkan_context.h"

namespace zview {

// Upload `size` bytes from `data` into a GPU-local VkBuffer via a staging
// buffer. Destroys any existing buffer first if buffer != VK_NULL_HANDLE.
inline bool uploadBuffer(const void* data, VkDeviceSize size,
                         VkBufferUsageFlags usage, VkBuffer& buffer,
                         VmaAllocation& alloc) {
  auto& ctx = VulkanContext::get();

  // Destroy old buffer if present
  if (buffer != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(ctx.device);
    vmaDestroyBuffer(ctx.allocator, buffer, alloc);
    buffer = VK_NULL_HANDLE;
    alloc = VK_NULL_HANDLE;
  }

  // Create staging buffer (CPU-visible)
  VkBufferCreateInfo stageBci{};
  stageBci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  stageBci.size = size;
  stageBci.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  VmaAllocationCreateInfo stageAci{};
  stageAci.usage = VMA_MEMORY_USAGE_CPU_ONLY;
  VkBuffer stageBuf;
  VmaAllocation stageAlloc;
  if (vmaCreateBuffer(ctx.allocator, &stageBci, &stageAci, &stageBuf,
                      &stageAlloc, nullptr) != VK_SUCCESS)
    return false;

  void* mapped;
  vmaMapMemory(ctx.allocator, stageAlloc, &mapped);
  std::memcpy(mapped, data, size);
  vmaUnmapMemory(ctx.allocator, stageAlloc);

  // Create GPU-local buffer
  VkBufferCreateInfo gpuBci{};
  gpuBci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  gpuBci.size = size;
  gpuBci.usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  VmaAllocationCreateInfo gpuAci{};
  gpuAci.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  if (vmaCreateBuffer(ctx.allocator, &gpuBci, &gpuAci, &buffer, &alloc,
                      nullptr) != VK_SUCCESS) {
    vmaDestroyBuffer(ctx.allocator, stageBuf, stageAlloc);
    return false;
  }

  // Copy staging → GPU
  VkCommandBuffer cmd = ctx.beginOneShot();
  VkBufferCopy region{0, 0, size};
  vkCmdCopyBuffer(cmd, stageBuf, buffer, 1, &region);
  ctx.endOneShot(cmd);

  vmaDestroyBuffer(ctx.allocator, stageBuf, stageAlloc);
  return true;
}

}  // namespace zview
