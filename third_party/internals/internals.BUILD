
cc_library(
    name = "libglew",
    srcs = glob(["lib/x86_64-linux-gnu/libGLEW.*"]),
    hdrs = glob(["include/GL/*.h"]),
    linkopts = [
        "-lGLEW",
        "-lGL",
        "-ldl",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "libglfw",
    srcs = glob(["lib/x86_64-linux-gnu/libglfw.so.*"]),
    hdrs = glob(["include/GLFW/*.h"]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "vulkan",
    srcs = glob(["lib/x86_64-linux-gnu/libvulkan.so.*"]),
    hdrs = glob(["include/vulkan/*.h"]),
    visibility = ["//visibility:public"],
)
