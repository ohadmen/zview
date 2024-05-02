load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "io",
    srcs = [
        "read_ply.cpp",
        "write_ply.cpp",

    ],
    hdrs = ["read_ply.h",
    "write_ply.h",
            ],
    
    deps = [
        "//src/types",
        "//src/io:read_file"
    ],
        visibility = ["//visibility:public"],

)
cc_library(
    name = "read_file",
    srcs = [
        "read_file.cpp",
    ],
    hdrs = ["read_file.h"],
    deps = [

    ],
        visibility = ["//visibility:public"],
)