load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "io",
    srcs = [
        "read_ply.cpp",
        "write_ply.cpp",
        "read_file.cpp",

    ],
    hdrs = ["read_ply.h",
    "write_ply.h",
    "read_file.h",
            ],
    
    deps = [
        "//src/types",
    ],
        visibility = ["//visibility:public"],

)
