load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "test",
    srcs = [
        "main.cpp",
        "imgui_impl_glfw.cpp",
"imgui_impl_glfw.h",
"imgui_impl_opengl3.cpp",
"imgui_impl_opengl3.h",
"imgui_impl_opengl3_loader.h",
"opengl_shader.cpp",
"opengl_shader.h",
"file_manager.cpp",
"file_manager.h",


    ],
    
    deps = [
        "@imgui",
        "@sysroot//:libglew",
        "@sysroot//:libglfw"

    ],
    data = glob(["assets/*"]),
    
)
