load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "zview",
    srcs = [
        "zview_main.cpp",  
    ],
    data = ["//shaders"],

    deps = [
        "//src/opengl_backend",
        "//src/io",
        "//src/drawables",
        "//src/drawables:vp_mat",
        "//src/ui:input_device_handler"
    ],
)
