load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")
load("@python_pip_deps//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library", "py_test")
load("@rules_python//python:packaging.bzl", "py_package")

pybind_extension(
    name = "pyzview_inf",
    srcs = ["pyzview_inf.cpp"],
    deps = ["//zview/io/shared_memory:client"],
)

py_library(
    name = "pyzview",
    srcs = [
        "__init__.py",
        "pyzview.py",
    ],
    data = [":pyzview_inf"],
)

# Use py_package to collect all transitive dependencies of a target,
# selecting just the files within a specific python package.
py_package(
    name = "pyzview_pkg",
    visibility = ["//visibility:public"],
    # Only include these Python packages.
    deps = [":pyzview"],
)

py_test(
    name = "pyzview_test",
    srcs = ["tests/pyzview_test.py"],
    deps = [
        requirement("numpy"),
        ":pyzview",
    ],
)
