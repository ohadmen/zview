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
    name = "picking_texture",
    srcs = [
        "picking_texture.cpp",
    ],
    hdrs = ["picking_texture.h"],
    deps = [
        "//src/opengl_backend",
    ],
            visibility = ["//visibility:public"],
)


cc_library(
    name = "shape_interface",
    srcs = [
        "shape_init_visitor.cpp",
        "shape_draw_visitor.cpp",
        "picking_texture.cpp",
    ],
    hdrs = ["shape_init_visitor.h","shape_draw_visitor.h","picking_texture.h"],

    deps = [
        "//src/params",
        "//src/types",
        "//src/io",
        

    ],
            visibility = ["//visibility:__pkg__"],

)


cc_library(
    name = "backdrop",
    srcs = [
        "backdrop.cpp"
    ],
    hdrs = ["backdrop.h"],

    deps = [
        "//src/types",
        "//src/drawables:shape_interface"
        

    ],
            visibility = ["//visibility:public"],

)

cc_library(
    name = "grid",
    srcs = [
        "grid.cpp"
    ],
    hdrs = ["grid.h"],

    deps = [
        "//src/params",
        "//src/geometry:mvp_mat",
        "//src/types",
        "//src/drawables:shape_interface"
        

    ],
            visibility = ["//visibility:public"],

)

cc_library(
    name = "axis",
    srcs = [
        "axis.cpp"
    ],
    hdrs = ["axis.h"],

    deps = [
        "//src/params",
        "//src/geometry:mvp_mat",
        "//src/types",
        "//src/drawables:shape_interface"
        

    ],
            visibility = ["//visibility:public"],

)
