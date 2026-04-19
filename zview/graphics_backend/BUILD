load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "graphics_backend",
    visibility = ["//visibility:public"],
    deps = [
        "@imgui",
        "@imgui//:imgui_glfw_backend",
        "@imgui//:imgui_vulkan_backend",
        "@sysroot//:libglfw",
        "@sysroot//:vulkan",
    ],
)

cc_library(
    name = "vulkan_context",
    srcs = ["vulkan_context.cpp"],
    hdrs = ["vulkan_context.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@imgui//:imgui_vulkan_backend",
        "@sysroot//:libglfw",
        "@sysroot//:vulkan",
        "@vma",
    ],
)

cc_library(
    name = "vk_buffer_utils",
    hdrs = ["vk_buffer_utils.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":vulkan_context",
        "@sysroot//:vulkan",
        "@vma",
    ],
)

cc_library(
    name = "shader",
    srcs = ["shader.cpp"],
    hdrs = ["shader.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":vulkan_context",
        "//zview/graphics_backend/shader_code:shader_spirv",
        "@sysroot//:vulkan",
    ],
)

cc_library(
    name = "frame_buffer",
    srcs = ["frame_buffer.cpp"],
    hdrs = ["frame_buffer.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":vulkan_context",
        "@imgui//:imgui_vulkan_backend",
        "@sysroot//:vulkan",
        "@vma",
    ],
)
