load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "zview_binary",
    srcs = [
        "zview_binary.cpp",
    ],
    deps = [
        "//src:zview",
        "//src/graphics_backend",
        "//src/types:external_types",
        "@sysroot//:libglew",
        "@sysroot//:libglfw",
    ],
)

cc_library(
    name = "zview",
    srcs = ["zview.cpp"],
    hdrs = ["zview.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//src/drawables",
        "//src/io",
        "//src/params",
        "//src/ui:input_device_handler",
        "//src/ui:zview_main_app",
        "//src/utils:recast",
    ],
)
