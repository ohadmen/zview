load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "io",
    srcs = [
        "read_ply.cpp",
        "write_ply.cpp",
    ],
    hdrs = [
        "read_ply.h",
        "write_ply.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/io:read_file",
        "//src/types",
        "//src/utils:recast",
    ],
)

cc_library(
    name = "read_file",
    srcs = [
        "read_file.cpp",
    ],
    hdrs = ["read_file.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//src/utils:recast",
    ],
)

cc_library(
    name = "shared_mem",
    srcs = [
    ],
    hdrs = ["shared_mem.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//src/types",
    ],
)
