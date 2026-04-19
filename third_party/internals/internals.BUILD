
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "libglfw",
    srcs = glob(["lib/x86_64-linux-gnu/libglfw.so.*"]),
    hdrs = glob(["include/GLFW/*.h"]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "vulkan",
    srcs = glob(["lib/x86_64-linux-gnu/libvulkan.so.*"]),
    hdrs = glob([
        "include/vulkan/*.h",
        "include/vk_video/*.h",
    ]),
    linkopts = ["-lvulkan"],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "glslangValidator",
    srcs = ["bin/glslangValidator"],
    visibility = ["//visibility:public"],
)
