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
        "shape_init.cpp"
    ],
    hdrs = ["shape_init.h"],

    deps = [
        "//src/types"
    ],
            visibility = ["//visibility:__pkg__"],

)

