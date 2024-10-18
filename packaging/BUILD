load("@bazel_skylib//rules:write_file.bzl", "write_file")
load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_pkg//pkg:deb.bzl", "pkg_deb")
load("@rules_pkg//pkg:mappings.bzl", "pkg_attributes", "pkg_files", "strip_prefix")
load("@rules_pkg//pkg:tar.bzl", "pkg_tar")
load("@rules_python//python:packaging.bzl", "py_wheel")
load("//packaging:zview_version.bzl", "ZVIEW_VERSION")

write_file(
    name = "gen_desktop_link",
    out = "zview.desktop",
    content = [
        "[Desktop Entry]",
        "Version=" + ZVIEW_VERSION,
        "Name=zview",
        "Comment=zview",
        "Exec=/usr/bin/zview",
        "Icon=/usr/share/pixmaps/zview.png",
        "Terminal=false",
        "Type=Application",
        "Categories=Utility;Application;",
        "StartupNotify=true",
        "StartupWMClass=zview " + ZVIEW_VERSION,
    ],
)

cc_binary(
    name = "zview",
    deps = ["//zview:zview_binary"],
)

pkg_files(
    name = "binary",
    srcs = [
        "//packaging:zview",
    ],
    attributes = pkg_attributes(
        group = "root",
        mode = "0551",
        owner = "root",
    ),
    prefix = "/usr/bin",
)

pkg_files(
    name = "binary_icon",
    srcs = [
        ":zview.png",
    ],
    attributes = pkg_attributes(
        group = "root",
        mode = "0644",
    ),
    # Where it should be in the final package
    prefix = "/usr/share/pixmaps",
    # Required, but why?: see #354
    strip_prefix = strip_prefix.from_pkg(),
)

pkg_files(
    name = "binary_desktoplink",
    srcs = [
        ":zview.desktop",
    ],
    attributes = pkg_attributes(
        group = "root",
        mode = "0644",
    ),

    # Where it should be in the final package
    prefix = "/usr/share/applications",
    # Required, but why?: see #354
    strip_prefix = strip_prefix.from_pkg(),
)

pkg_tar(
    name = "tarball",
    srcs = [
        ":binary",
        ":binary_desktoplink",
        ":binary_icon",
    ],
)

py_wheel(
    name = "wheel",
    abi = None,
    author = "Ohad Menashe",
    author_email = "ohad.men@gmail.com",
    distribution = "pyzview",
    license = "Apache 2.0",
    python_requires = ">=3.9.0",
    python_tag = "cp311",
    twine = "@publish_deps//twine",
    version = ZVIEW_VERSION,
    deps = ["//pyzview:pyzview_pkg"],
)

pkg_deb(
    name = "debian-package",
    data = ":tarball",
    description_file = "description.txt",
    distribution = "jammy",
    maintainer = "Ohad.men@gmail.com",
    package = "zview",
    version = ZVIEW_VERSION,
)
