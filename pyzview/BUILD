load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")
load("@python_pip_deps//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library", "py_test")

pybind_extension(
    name = "pyzview_inf",
    srcs = ["pyzview_inf.cpp"],
    deps = ["//zview/io/shared_memory:client"],
)

py_library(
    name = "pyzview_lib",
    data = [":pyzview_inf"],
    imports = ["."],
)

py_test(
    name = "pyzview_test",
    srcs = ["pyzview_test.py"],
    deps = [
        ":pyzview_lib",
        requirement("numpy"),
    ],
)
