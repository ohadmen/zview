load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "zview",
    srcs = [
        "zview_main.cpp",  
    ],
    data = ["//shaders"],

    deps = [
        "//src/ui:main_app",
        "//src/opengl_backend",
        "//src/io",
        "//src/drawables",
        "//src/ui:input_device_handler",
        "//src/params"
    ],
)
