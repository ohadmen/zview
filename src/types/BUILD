load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "external_types",
    hdrs = [
        "external_types.h",
    ],
    visibility = ["//visibility:public"],
)

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
        "//src/types:external_types",
        "@eigen",
    ],
)
