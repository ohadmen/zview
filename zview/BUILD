load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "zview_binary",
    srcs = [
        "zview_binary.cpp",
    ],
    deps = [
        "//zview",
        "//zview/graphics_backend",
        "//zview/types:external_types",
        "@sysroot//:libglew",
        "@sysroot//:libglfw",
    ],
)

cc_library(
    name = "zview",
    srcs = ["zview_inf.cpp"],
    hdrs = ["zview_inf.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//zview:zview_inf_impl",
    ],
)

cc_library(
    name = "zview_inf_impl",
    srcs = ["zview_inf_impl.cpp"],
    hdrs = [
        "zview_inf.h",
        "zview_inf_impl.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//zview/drawables",
        "//zview/graphics_backend:frame_buffer",
        "//zview/io",
        "//zview/params",
        "//zview/ui:input_device_handler",
        "//zview/ui:tree_view",
        "@imgui_file_dialog",
    ],
)
