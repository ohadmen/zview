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
    ],
)

cc_library(
    name = "shader",
    srcs = ["shader.cpp"],
    hdrs = ["shader.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//src/graphics_backend",
        "//src/graphics_backend/shader_code",
    ],
)
