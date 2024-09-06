load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")
load("@rules_python//python:defs.bzl", "py_library")
load("@rules_python//python:packaging.bzl", "py_package", "py_wheel")

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
    visibility = ["//visibility:private"],
    # Only include these Python packages.
    deps = [":pyzview"],
)

py_wheel(
    name = "wheel",
    abi = None,
    author = "Ohad Menashe",
    author_email = "ohad.men@gmail.com",
    distribution = "pyzview",
    license = "Apache 2.0",
    platform = select({
        "@bazel_tools//src/conditions:linux_x86_64": "linux_x86_64",
    }),
    python_requires = ">=3.9.0",
    python_tag = "cp311",
    twine = "@publish_deps//twine",
    version = "0.0.1",
    deps = [":pyzview_pkg"],
)

# py_import(name="runfiles", whl=":wheel")

# py_test(
#     name = "pyzview_test",
#     srcs = ["tests/pyzview_test.py"],
#     deps = [
#           requirement("numpy"),
#     ],
#     imports = ["../../"],

# )
