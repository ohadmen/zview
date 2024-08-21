load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "implot",
    srcs = [
        "implot.cpp",
        "implot_demo.cpp",
        "implot_items.cpp",
    ],
    hdrs = [
        "implot.h",
        "implot_internal.h",
    ],
    include_prefix = "implot",
    includes = ["."],
    deps = ["@imgui"],
)
