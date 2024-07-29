load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "zview_binary",
    srcs = [
        "zview_main.cpp",
    ],
    deps = [
        "//src:zview",
        "//src/types:external_types",
    ],
)

cc_library(
    name = "zview",
    srcs = ["zview.cpp"],
    hdrs = ["zview.h"],
    data = ["//shaders"],
    visibility = ["//visibility:public"],
    deps = [
        "//src/drawables",
        "//src/graphics_backend",
        "//src/io",
        "//src/params",
        "//src/ui:input_device_handler",
        "//src/ui:main_app",
    ],
)

cc_binary(
    name = "zview_interface_test",
    srcs = ["zview_interface_test.cpp"],
    deps = [
        "//src:zview",
        "//src/graphics_backend",
        "//src/types:external_types",
        "@imgui",
    ],
)
