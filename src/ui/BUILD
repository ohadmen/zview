

cc_library(
    name = "main_app",
        

    deps = [
        "//src/opengl_backend",
        "//src/io",
        "//src/drawables",
        "//src/ui:input_device_handler",
        "//src/params",
        "//src/drawables:backdrop",
        "//src/drawables:grid",
        "//src/drawables:axis",
        "//src/ui:tree_view"
    ],

    srcs = ["main_app.cpp"],
    hdrs = ["main_app.h"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "input_device_handler",
    srcs = [
        "input_device_handler.cpp"
    ],
    hdrs = [
        "input_device_handler.h",
    ],
    deps = ["@imgui","//src/geometry:mvp_mat","//src/params"],
    visibility = ["//visibility:public"],
    

)


cc_library(
    name = "tree_view",
    srcs = [
        "tree_view.cpp"
    ],
    hdrs = [
        "tree_view.h",
    ],
    deps = ["@imgui"],
    visibility = ["//visibility:public"],
    

)



