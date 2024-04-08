load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "common_types",
    srcs = [
        "common_types.cpp",
    ],
    hdrs = [
        "common_types.h",
    ],
    
    deps = [
        "@eigen",
    ],
        visibility = ["//visibility:public"],

)

cc_library(
    name = "params",
    srcs = [
        "params.cpp"
    ],
    hdrs = [
        "params.h",
        "version.h",
    ],
    
    deps = [
        "common_types",
    ],
        visibility = ["//visibility:public"],

)

cc_library(
    name = "memstream",
    srcs = [
        "memstream.cpp"
    ],
    hdrs = [
        "memstream.h",
    ],
        visibility = ["//visibility:public"],

    

)
