load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "server",
    srcs = [
        "shared_memory_server.cpp",
    ],
    hdrs = ["shared_memory_server.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//zview/io/shared_memory:shared_memory_types",
        "//zview/types",
        "//zview/utils:recast",
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
        "//zview/io/shared_memory:shared_memory_types",
        "//zview/types:external_types",
        "//zview/utils:recast",
    ],
)

cc_library(
    name = "shared_memory_types",
    hdrs = ["shared_memory_types.h"],
    visibility = ["//visibility:__pkg__"],
)

cc_test(
    name = "interprocess_test",
    srcs = ["interprocess_test.cpp"],
    deps = [
        "@boost//:interprocess",
    ],
)
