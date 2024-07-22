load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "graphics_backend",
    srcs = [
        "imgui_impl_glfw.cpp",
        "imgui_impl_opengl3.cpp",
        "imgui_impl_vulkan.cpp",
        "opengl_shader.cpp"
    ],
    hdrs = ["imgui_impl_glfw.h",
    "imgui_impl_opengl3.h",
    "imgui_impl_opengl3_loader.h", 
    "opengl_shader.h",
    "imgui_impl_vulkan.h",
    ],
    
    deps = [
        "@eigen",
        "@imgui",
        "@sysroot//:libglew",
        "@sysroot//:libglfw",
        "@sysroot//:vulkan",
                "//src/io:read_file",
                


    ],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "shader",
    deps = ["//src/graphics_backend"],
    srcs = ["shader.cpp"],
    hdrs = ["shader.h"],
    visibility = ["//visibility:public"],
)
