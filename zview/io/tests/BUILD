load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "read_file_test",
    srcs = ["read_file_test.cpp"],
    data = ["//models"],
    defines = ["CATCH_CONFIG_MAIN"],
    deps = [
        "//zview/io",
        "@catch2//:catch2_main",
    ],
)
