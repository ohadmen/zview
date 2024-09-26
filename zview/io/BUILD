load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "io",
    srcs = [
        "read_file.cpp",
        "reader_pcd.cpp",
        "reader_ply.cpp",
        "write_ply.cpp",
    ],
    hdrs = [
        "read_file.h",
        "reader_abs.h",
        "reader_pcd.h",
        "reader_ply.h",
        "write_ply.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//zview/types",
        "//zview/utils:recast",
    ],
)
