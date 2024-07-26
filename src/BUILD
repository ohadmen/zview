load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "zview",
    srcs = [
        "zview_main.cpp",
    ],
    data = ["//shaders"],
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
    name = "vulkan_test",
    srcs = ["vulkan_test.cpp"],
    deps = ["//src/graphics_backend"],
)
