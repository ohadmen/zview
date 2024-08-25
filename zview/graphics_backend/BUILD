load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "graphics_backend",
    srcs = [
        "imgui_impl_glfw.cpp",
        "imgui_impl_opengl3.cpp",
    ],
    hdrs = [
        "imgui_impl_glfw.h",
        "imgui_impl_opengl3.h",
        "imgui_impl_opengl3_loader.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@eigen",
        "@imgui",
        "@sysroot//:libglew",
        "@sysroot//:libglfw",
    ],
)

cc_library(
    name = "shader",
    srcs = ["shader.cpp"],
    hdrs = ["shader.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//zview/graphics_backend",
        "//zview/graphics_backend/shader_code",
    ],
)

cc_library(
    name = "frame_buffer",
    srcs = ["frame_buffer.cpp"],
    hdrs = ["frame_buffer.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//zview/graphics_backend",
    ],
)
