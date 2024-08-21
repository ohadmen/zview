load("@rules_cc//cc:defs.bzl", "cc_library")

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
        "//zview/drawables:statusbar",
        "//zview/geometry:mvp_mat",
        "//zview/params",
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
