#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <algorithm>
#include <iostream>
#include <random>

#include "zview/graphics_backend/vulkan_context.h"
#include "zview/zview_inf.h"

// NOLINTNEXTLINE
#define STR_(x) #x
// NOLINTNEXTLINE
#define STR(x) STR_(x)

static void glfw_error_callback(int error, const char *description) {
  std::cerr << "Glfw Error " << error << ": " << description << std::endl;
}

std::vector<zview::Face> generateRandomFaces(int n_vertices, int n_faces) {
  static std::default_random_engine gen;
  static std::uniform_int_distribution<std::uint32_t> dist(0, n_vertices - 1);
  std::vector<zview::Face> faces{static_cast<std::size_t>(n_faces)};
  std::for_each(faces.begin(), faces.end(), [&](zview::Face &f) {
    f = {dist(gen), dist(gen), dist(gen)};
  });
  return faces;
}

std::vector<zview::Edge> generateRandomEdges(int n_vertices, int n_edges) {
  static std::default_random_engine gen;
  static std::uniform_int_distribution<std::uint32_t> dist(0, n_vertices - 1);
  std::vector<zview::Edge> edges{static_cast<std::size_t>(n_edges)};
  std::for_each(edges.begin(), edges.end(), [&](zview::Edge &e) {
    e = {dist(gen), dist(gen)};
  });
  return edges;
}

std::vector<zview::Vertex> generateRandomVertices(int n_vertices,
                                                  float z_offset) {
  std::vector<zview::Vertex> vertices{static_cast<std::size_t>(n_vertices)};
  static std::default_random_engine gen;
  static std::uniform_real_distribution<float> dist(-1, 1);
  auto random_color = []() {
    return static_cast<std::uint8_t>((dist(gen) / 2 + 0.5) * 255);
  };
  auto random_pos = [&]() { return dist(gen); };
  std::for_each(vertices.begin(), vertices.end(), [&](zview::Vertex &v) {
    v = {random_pos(),
         random_pos(),
         random_pos() + z_offset,
         random_color(),
         random_color(),
         random_color(),
         255U};
  });
  return vertices;
}

static std::vector<std::string> get_args(int argc, char **argv) {
  std::vector<std::string> list;
  for (int i = 1; i != argc; ++i) list.push_back(*std::next(argv, i));
  return list;
}

int main(int argc, char *argv[]) {
  const auto files = get_args(argc, argv);

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) return 1;

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  const char *version_string = STR(ZVIEW_VERSION);
  GLFWwindow *window = glfwCreateWindow(
      1280, 720, (std::string("zview ") + version_string).c_str(), nullptr,
      nullptr);
  if (!window) {
    std::cerr << "Failed to create window\n";
    return 1;
  }

  auto &ctx = zview::VulkanContext::get();
  if (!ctx.init(window)) {
    std::cerr << "Vulkan init failed\n";
    return 1;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui::StyleColorsDark();
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

  ImGui_ImplGlfw_InitForVulkan(window, true);
  ImGui_ImplVulkan_InitInfo vk_init{};
  vk_init.Instance = ctx.instance;
  vk_init.PhysicalDevice = ctx.physDevice;
  vk_init.Device = ctx.device;
  vk_init.QueueFamily = ctx.graphicsFamily;
  vk_init.Queue = ctx.graphicsQueue;
  vk_init.DescriptorPool = ctx.imguiDescPool;
  vk_init.MinImageCount = 2;
  vk_init.ImageCount = static_cast<uint32_t>(ctx.swapImages.size());
  vk_init.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  ImGui_ImplVulkan_Init(&vk_init, ctx.swapRenderPass);
  ImGui_ImplVulkan_CreateFontsTexture();

  auto app = zview::ZviewInf::create();
  if (!app->init()) return 1;

  glfwSetWindowUserPointer(window, app.get());
  glfwSetDropCallback(window, [](GLFWwindow *w, int count, const char **paths) {
    auto *a = static_cast<zview::ZviewInf *>(glfwGetWindowUserPointer(w));
    std::vector<std::string> fs(count);
    for (int i = 0; i < count; ++i) fs[i] = paths[i];
    a->loadFiles(fs);
  });

  app->loadFiles(files);

#ifdef DEBUG_DATA_INJECTION
  bool add_pcl{true}, add_mesh{false}, add_edges{false};
  int n_vertices{1000}, n_edges{1000}, n_faces{1000};
#endif

  bool swapchainDirty = false;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    if (swapchainDirty) {
      ctx.rebuildSwapchain(window);
      swapchainDirty = false;
      continue;
    }

    uint32_t frame = ctx.currentFrame;
    vkWaitForFences(ctx.device, 1, &ctx.inFlight[frame], VK_TRUE, UINT64_MAX);

    VkResult acqResult = vkAcquireNextImageKHR(
        ctx.device, ctx.swapchain, UINT64_MAX, ctx.imgAvailable[frame],
        VK_NULL_HANDLE, &ctx.imageIndex);
    if (acqResult == VK_ERROR_OUT_OF_DATE_KHR) {
      swapchainDirty = true;
      continue;
    }

    vkResetFences(ctx.device, 1, &ctx.inFlight[frame]);
    VkCommandBuffer cmd = ctx.cmdBuffers[frame];
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo bi{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(cmd, &bi);

    // ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("window", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_NoDocking |
                     ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::PopStyleVar();
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(io.DisplaySize);

    if (!app->draw()) break;  // records offscreen + picking into cmd

    ImGui::End();

#ifdef DEBUG_DATA_INJECTION
    ImGui::Begin("interface inject");
    ImGui::Checkbox("add pcl", &add_pcl);
    ImGui::SameLine();
    ImGui::SliderInt("n vertices", &n_vertices, 0, 10000);
    ImGui::Checkbox("add mesh", &add_mesh);
    ImGui::SameLine();
    ImGui::SliderInt("n faces", &n_faces, 0, 1000);
    ImGui::Checkbox("add edges", &add_edges);
    ImGui::SameLine();
    ImGui::SliderInt("n edges", &n_edges, 0, 1000);
    ImGui::End();
    if (add_pcl)
      app->plot("test_interface/pcl", generateRandomVertices(n_vertices, 0));
    if (add_mesh)
      app->plot("test_interface/mesh", generateRandomVertices(n_vertices, 2),
                generateRandomFaces(n_vertices, n_faces));
    if (add_edges)
      app->plot("test_interface/edges", generateRandomVertices(n_vertices, 4),
                generateRandomEdges(n_vertices, n_edges));
#endif

    ImGui::Render();

    // Swapchain render pass — ImGui draws to the swap image
    VkClearValue clearVal{{{0.f, 0.f, 0.f, 1.f}}};
    VkRenderPassBeginInfo rpbi{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rpbi.renderPass = ctx.swapRenderPass;
    rpbi.framebuffer = ctx.swapFramebuffers[ctx.imageIndex];
    rpbi.renderArea.extent = ctx.swapchainExtent;
    rpbi.clearValueCount = 1;
    rpbi.pClearValues = &clearVal;
    vkCmdBeginRenderPass(cmd, &rpbi, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport vp{0,
                  0,
                  (float)ctx.swapchainExtent.width,
                  (float)ctx.swapchainExtent.height,
                  0.f,
                  1.f};
    VkRect2D sc{{0, 0}, ctx.swapchainExtent};
    vkCmdSetViewport(cmd, 0, 1, &vp);
    vkCmdSetScissor(cmd, 0, 1, &sc);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
    vkCmdEndRenderPass(cmd);
    vkEndCommandBuffer(cmd);

    // Submit
    VkPipelineStageFlags waitStage =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo si{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    si.waitSemaphoreCount = 1;
    si.pWaitSemaphores = &ctx.imgAvailable[frame];
    si.pWaitDstStageMask = &waitStage;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &cmd;
    si.signalSemaphoreCount = 1;
    si.pSignalSemaphores = &ctx.renderDone[frame];
    vkQueueSubmit(ctx.graphicsQueue, 1, &si, ctx.inFlight[frame]);

    // Present
    VkPresentInfoKHR pi{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    pi.waitSemaphoreCount = 1;
    pi.pWaitSemaphores = &ctx.renderDone[frame];
    pi.swapchainCount = 1;
    pi.pSwapchains = &ctx.swapchain;
    pi.pImageIndices = &ctx.imageIndex;
    VkResult presResult = vkQueuePresentKHR(ctx.presentQueue, &pi);
    if (presResult == VK_ERROR_OUT_OF_DATE_KHR ||
        presResult == VK_SUBOPTIMAL_KHR)
      swapchainDirty = true;

    ctx.currentFrame = (frame + 1) % zview::kMaxFramesInFlight;
  }

  vkDeviceWaitIdle(ctx.device);
  // Reset app before ctx.destroy() so VMA allocations are freed first.
  app.reset();
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  ctx.destroy();

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
