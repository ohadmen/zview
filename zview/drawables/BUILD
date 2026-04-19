load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "drawables",
    visibility = ["//visibility:public"],
    deps = [
        ":axis",
        ":backdrop",
        ":grid",
        ":picking_texture",
        ":shape_buffer",
        ":statusbar",
    ],
)

cc_library(
    name = "shape_buffer",
    srcs = [
        "shape_buffer.cpp",
    ],
    hdrs = ["shape_buffer.h"],
    visibility = ["//visibility:__pkg__"],
    deps = [
        "//zview/drawables:shape_interface",
        "//zview/graphics_backend:vulkan_context",
        "//zview/types",
    ],
)

cc_library(
    name = "picking_texture",
    srcs = [
        "picking_texture.cpp",
    ],
    hdrs = ["picking_texture.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//zview/drawables:shape_interface",
        "//zview/graphics_backend:frame_buffer",
        "//zview/graphics_backend:shader",
        "//zview/graphics_backend:vulkan_context",
        "//zview/types",
        "@sysroot//:vulkan",
        "@vma",
    ],
)

cc_library(
    name = "shape_interface",
    srcs = [
        "shape_draw_visitor.cpp",
        "shape_init_visitor.cpp",
        "shape_update_visitor.cpp",
    ],
    hdrs = [
        "shape_draw_visitor.h",
        "shape_init_visitor.h",
        "shape_update_visitor.h",
    ],
    visibility = ["//visibility:__pkg__"],
    deps = [
        "//zview/graphics_backend:frame_buffer",
        "//zview/graphics_backend:shader",
        "//zview/graphics_backend:vk_buffer_utils",
        "//zview/graphics_backend:vulkan_context",
        "//zview/io",
        "//zview/params",
        "//zview/types",
        "//zview/utils:recast",
        "@sysroot//:vulkan",
        "@vma",
    ],
)

cc_library(
    name = "backdrop",
    srcs = [
        "backdrop.cpp",
    ],
    hdrs = ["backdrop.h"],
    visibility = ["//visibility:__pkg__"],
    deps = [
        "//zview/drawables:shape_interface",
        "//zview/graphics_backend:shader",
        "//zview/graphics_backend:vulkan_context",
        "//zview/types",
    ],
)

cc_library(
    name = "grid",
    srcs = [
        "grid.cpp",
    ],
    hdrs = ["grid.h"],
    visibility = ["//visibility:__pkg__"],
    deps = [
        "//zview/drawables:shape_interface",
        "//zview/geometry:mvp_mat",
        "//zview/graphics_backend:shader",
        "//zview/graphics_backend:vulkan_context",
        "//zview/params",
        "//zview/types",
    ],
)

cc_library(
    name = "axis",
    srcs = [
        "axis.cpp",
    ],
    hdrs = ["axis.h"],
    visibility = ["//visibility:__pkg__"],
    deps = [
        "//zview/drawables:shape_interface",
        "//zview/geometry:mvp_mat",
        "//zview/graphics_backend:shader",
        "//zview/graphics_backend:vulkan_context",
        "//zview/params",
        "//zview/types",
    ],
)

cc_library(
    name = "statusbar",
    srcs = [
        "statusbar.cpp",
    ],
    hdrs = ["statusbar.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@imgui",
    ],
)
