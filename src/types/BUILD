load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "types",
    srcs = [
        "types.cpp",
    ],
    hdrs = [
        "types.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/graphics_backend:shader",
        "@eigen",
    ],
)
