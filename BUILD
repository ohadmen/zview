load("@rules_python//python:packaging.bzl", "py_wheel")

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
    deps = ["//pyzview:pyzview_pkg"],
)
