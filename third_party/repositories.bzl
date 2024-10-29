"""Fetch external http repositories."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:local.bzl", "new_local_repository")


def _repositories_impl(ctx):
    http_archive(
        name = "imgui",
        build_file = "//third_party/imgui:imgui.BUILD",
        sha256 = "d4b7fd185443111a3a892d4625c85ab9666c6c9cb5484e3a447de6af419f8d2f",
        strip_prefix = "imgui-1.90-docking",
        url = "https://github.com/ocornut/imgui/archive/refs/tags/v1.90-docking.tar.gz",
    )

    http_archive(
        name = "implot",
        build_file = "//third_party/implot:implot.BUILD",
        sha256 = "961df327d8a756304d1b0a67316eebdb1111d13d559f0d3415114ec0eb30abd1",
        strip_prefix = "implot-0.16",
        url = "https://github.com/epezent/implot/archive/refs/tags/v0.16.tar.gz",
        patches = ["//third_party/implot:implot_fix_macro_redefinition.patch"],
    )

    # ImGui filedialog
    http_archive(
        name = "imgui_file_dialog",
        sha256 = "136e714965afaec2bac857bf46a653fdd74a0bf493e281682706c604113026b8",
        strip_prefix = "ImGuiFileDialog-0.6.7",
        url = "https://github.com/aiekick/ImGuiFileDialog/archive/refs/tags/v0.6.7.tar.gz",
        build_file = "//third_party/imgui_file_dialog:imgui_file_dialog.BUILD",
    )

    new_local_repository(
        name = "sysroot",
        build_file = "@//third_party/internals:internals.BUILD",
        path = "/usr",
    )


repositories_impl = module_extension(implementation = _repositories_impl)
