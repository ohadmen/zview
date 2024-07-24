load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "main_app",
    srcs = ["main_app.cpp"],
    hdrs = ["main_app.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//src/drawables",
        "//src/drawables:axis",
        "//src/drawables:backdrop",
        "//src/drawables:grid",
        "//src/graphics_backend",
        "//src/io",
        "//src/params",
        "//src/ui:input_device_handler",
        "//src/ui:tree_view",
    ],
)

cc_library(
    name = "input_device_handler",
    srcs = [
        "input_device_handler.cpp",
    ],
    hdrs = [
        "input_device_handler.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/geometry:mvp_mat",
        "//src/params",
        "@imgui",
    ],
)

cc_library(
    name = "tree_view",
    srcs = [
        "tree_view.cpp",
    ],
    hdrs = [
        "tree_view.h",
    ],
    visibility = ["//visibility:public"],
    deps = ["@imgui"],
)
