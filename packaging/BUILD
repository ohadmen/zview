load("@rules_pkg//pkg:deb.bzl", "pkg_deb")
load("@rules_pkg//pkg:tar.bzl", "pkg_tar")
load("@rules_python//python:packaging.bzl", "py_wheel")

VERSION = "1.0.0"

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
    version = VERSION,
    deps = ["//pyzview:pyzview_pkg"],
)

pkg_tar(
    name = "tarball",
    srcs = ["//zview:zview_binary"],
)

pkg_deb(
    name = "debian-data",
    data =
        ":tarball",
    description_file = "description.md",
    maintainer = "Ohad Menashe",
    package = "zview",
    version = VERSION,
)
