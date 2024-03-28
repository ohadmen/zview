load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "zview",
    srcs = [
        "gui/zview_main.cpp",
    ],
    
    deps = [
        "@imgui",
        "@sysroot//:libglew",
        "@sysroot//:libglfw"

    ],
)
