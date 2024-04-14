load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "drawables",
    srcs = [
        "shape_buffer.cpp",  

    ],
    hdrs = ["shape_buffer.h"],
    

    deps = [
        "//src/types",
        "//src/drawables:shape_interface"
    ],
            visibility = ["//visibility:public"],

)

cc_library(
    name = "shape_interface",
    srcs = [
        "shape_init_visitor.cpp",
        "shape_draw_visitor.cpp"
    ],
    hdrs = ["shape_init_visitor.h","shape_draw_visitor.h"],

    deps = [
        "//src/types",
        "//src/opengl_backend",
    ],
            visibility = ["//visibility:__pkg__"],

)

