load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "server",
    srcs = [
        "shared_memory_server.cpp",
    ],
    hdrs = [
        "pointer_shape_conversion.h",
        "shared_memory_server.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":shared_memory_types",
        "//zview/types",
        "//zview/utils:recast",
        "@boost//:interprocess",
    ],
)

cc_library(
    name = "client",
    srcs = [
        "shared_memory_client.cpp",
    ],
    hdrs = ["shared_memory_client.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":shared_memory_types",
        "//zview/types:external_types",
        "//zview/utils:recast",
        "@boost//:interprocess",
    ],
)

cc_library(
    name = "shared_memory_types",
    srcs = ["shared_memory_types.cpp"],
    hdrs = ["shared_memory_types.h"],
    visibility = ["//visibility:__subpackages__"],
    deps = [
        "//zview/utils:recast",
        "@boost//:interprocess",
    ],
)
