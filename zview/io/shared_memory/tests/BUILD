load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "shared_memory_test",
    srcs = ["shared_memory_test.cpp"],
    defines = ["CATCH_CONFIG_MAIN"],
    deps = [
        "//zview/io/shared_memory:client",
        "//zview/io/shared_memory:server",
        "@catch2//:catch2_main",
    ],
)
