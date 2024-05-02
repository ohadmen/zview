

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
        "//src/drawables:axis"
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



