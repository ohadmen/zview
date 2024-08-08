load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "drawables",
    srcs = [
        "shape_buffer.cpp",
    ],
    hdrs = ["shape_buffer.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//src/drawables:shape_interface",
        "//src/types",
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
        "//src/graphics_backend:shader",
        "//src/types",
    ],
)

cc_library(
    name = "shape_interface",
    srcs = [
        "picking_texture.cpp",
        "shape_draw_visitor.cpp",
        "shape_init_visitor.cpp",
        "shape_update_visitor.cpp",
    ],
    hdrs = [
        "picking_texture.h",
        "shape_draw_visitor.h",
        "shape_init_visitor.h",
        "shape_update_visitor.h",
    ],
    visibility = ["//visibility:__pkg__"],
    deps = [
        "//src/graphics_backend:frame_buffer",
        "//src/io",
        "//src/params",
        "//src/types",
        "//src/utils:recast",
    ],
)

cc_library(
    name = "backdrop",
    srcs = [
        "backdrop.cpp",
    ],
    hdrs = ["backdrop.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//src/drawables:shape_interface",
        "//src/types",
    ],
)

cc_library(
    name = "grid",
    srcs = [
        "grid.cpp",
    ],
    hdrs = ["grid.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//src/drawables:shape_interface",
        "//src/geometry:mvp_mat",
        "//src/params",
        "//src/types",
    ],
)

cc_library(
    name = "axis",
    srcs = [
        "axis.cpp",
    ],
    hdrs = ["axis.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//src/drawables:shape_interface",
        "//src/geometry:mvp_mat",
        "//src/params",
        "//src/types",
    ],
)
