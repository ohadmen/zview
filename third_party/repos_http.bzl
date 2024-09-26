"""Fetch external http repositories."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def fetch_http_repositories(name):
    """Fetch external http repositories.

    Args:
        name: unused.
    """

    # Make sure we follow the `docking` branch for multi-port and docking support!
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

    # Hedron's Compile Commands Extractor for Bazel
    # https://github.com/hedronvision/bazel-compile-commands-extractor
    http_archive(
        name = "hedron_compile_commands",

        # Replace the commit hash (0e990032f3c5a866e72615cf67e5ce22186dcb97) in both places (below) with the latest (https://github.com/hedronvision/bazel-compile-commands-extractor/commits/main), rather than using the stale one here.
        # Even better, set up Renovate and let it do the work for you (see "Suggestion: Updates" in the README).
        url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/0e990032f3c5a866e72615cf67e5ce22186dcb97.tar.gz",
        strip_prefix = "bazel-compile-commands-extractor-0e990032f3c5a866e72615cf67e5ce22186dcb97",
        # When you first run this tool, it'll recommend a sha256 hash to put here with a message like: "DEBUG: Rule 'hedron_compile_commands' indicated that a canonical reproducible form can be obtained by modifying arguments sha256 = ..."
        sha256 = "2b3ee8bba2df4542a508b0289727b031427162b4cd381850f89b406445c17578",
    )

    # Eigen
    http_archive(
        name = "eigen",
        sha256 = "8586084f71f9bde545ee7fa6d00288b264a2b7ac3607b974e54d13e7162c1c72",
        strip_prefix = "eigen-3.4.0",
        url = "https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz",
        build_file = "//third_party/eigen:eigen.BUILD",
    )

    # ImGui filedialog
    http_archive(
        name = "imgui_file_dialog",
        sha256 = "136e714965afaec2bac857bf46a653fdd74a0bf493e281682706c604113026b8",
        strip_prefix = "ImGuiFileDialog-0.6.7",
        url = "https://github.com/aiekick/ImGuiFileDialog/archive/refs/tags/v0.6.7.tar.gz",
        build_file = "//third_party/imgui_file_dialog:imgui_file_dialog.BUILD",
    )

    native.new_local_repository(
        name = "sysroot",
        build_file = "@//third_party/internals:internals.BUILD",
        path = "/usr",
    )
