load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "imgui",
    srcs = glob([
        "*.cpp",
    ]),
    hdrs = [
        "imconfig.h",
        "imgui.h",
        "imgui_internal.h",
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h",
    ],
    defines = [
        "IMGUI_DEFINE_MATH_OPERATORS",
        "ImDrawIdx=\"unsigned int\"",
    ],
    include_prefix = "imgui",
    includes = ["."],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "imgui_glfw_backend",
    srcs = ["backends/imgui_impl_glfw.cpp"],
    hdrs = ["backends/imgui_impl_glfw.h"],
    strip_include_prefix = "backends",
    include_prefix = "",
    includes = ["backends"],
    deps = [
        ":imgui",
        "@sysroot//:libglfw",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "imgui_vulkan_backend",
    srcs = ["backends/imgui_impl_vulkan.cpp"],
    hdrs = ["backends/imgui_impl_vulkan.h"],
    strip_include_prefix = "backends",
    include_prefix = "",
    includes = ["backends"],
    deps = [
        ":imgui",
        "@sysroot//:vulkan",
    ],
    visibility = ["//visibility:public"],
)
