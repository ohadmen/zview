load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "imgui_file_dialog",
    srcs = ["ImGuiFileDialog.cpp"],
    hdrs = [
        "ImGuiFileDialog.h",
        "ImGuiFileDialogConfig.h",
    ],
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = ["@imgui"],
)
