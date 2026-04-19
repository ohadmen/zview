load("@rules_cc//cc:defs.bzl", "cc_library")

SHADERS = [
    (
        "pcl",
        [
            "vert",
            "frag",
        ],
    ),
    (
        "mesh",
        [
            "vert",
            "frag",
        ],
    ),
    (
        "edges",
        [
            "vert",
            "frag",
        ],
    ),
    (
        "picking",
        [
            "vert",
            "frag",
        ],
    ),
    (
        "grid",
        [
            "vert",
            "frag",
        ],
    ),
]

[
    genrule(
        name = "{}_{}_spv".format(shader, stage),
        srcs = ["{}.{}".format(shader, stage)],
        outs = ["{}.{}.spv".format(shader, stage)],
        cmd = "$(execpath @sysroot//:glslangValidator) -V -o $@ $(SRCS)",
        tools = ["@sysroot//:glslangValidator"],
    )
    for shader, stages in SHADERS
    for stage in stages
]

[
    genrule(
        name = "{}_{}_h".format(shader, stage),
        srcs = ["{}.{}.spv".format(shader, stage)],
        outs = ["{}.{}.spv.h".format(shader, stage)],
        cmd = "$(execpath //tools:spv_to_header) $(SRCS) $@ {}_{}".format(shader, stage),
        tools = ["//tools:spv_to_header"],
    )
    for shader, stages in SHADERS
    for stage in stages
]

cc_library(
    name = "shader_spirv",
    hdrs = [
        "{}.{}.spv.h".format(shader, stage)
        for shader, stages in SHADERS
        for stage in stages
    ],
    visibility = ["//zview/graphics_backend:__pkg__"],
)
