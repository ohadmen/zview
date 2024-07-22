load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "zview",
    srcs = [
        "zview_main.cpp",  
    ],
    data = ["//shaders"],

    deps = [
        "//src/ui:main_app",
        "//src/graphics_backend",
        "//src/io",
        "//src/drawables",
        "//src/ui:input_device_handler",
        "//src/params"
    ],
)

cc_binary(
    name = "vulkan_test",
    deps = ["//src/graphics_backend"],

    srcs = ["vulkan_test.cpp"],
   
)