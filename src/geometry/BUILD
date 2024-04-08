load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "mvp_mat",
    srcs = [
        "mvp_mat.cpp",
    ],
    hdrs = [
        "mvp_mat.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/params",
        "//src/types",
        "@imgui",
    ],
)
