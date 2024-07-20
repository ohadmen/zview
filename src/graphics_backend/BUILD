load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "graphics_backend",
    srcs = [
        "imgui_impl_glfw.cpp",
        "imgui_impl_opengl3.cpp",
        "opengl_shader.cpp"
    ],
    hdrs = ["imgui_impl_glfw.h",
    "imgui_impl_opengl3.h",
    "imgui_impl_opengl3_loader.h", 
    "opengl_shader.h"
    ],
    
    deps = [
        "@eigen",
        "@imgui",
        "@sysroot//:libglew",
        "@sysroot//:libglfw",
                "//src/io:read_file",
                


    ],
    visibility = ["//visibility:public"],
)
