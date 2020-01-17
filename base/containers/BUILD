load("//bazel:base_cc.bzl", "base_cc_library")

base_cc_library(
    name = "flat_tree",
    hdrs = ["flat_tree.h"],
    deps = ["//base:stl_util"],
)

base_cc_library(
    name = "flat_map",
    hdrs = ["flat_map.h"],
    visibility = ["//visibility:public"],
    deps = [":flat_tree"],
)

base_cc_library(
    name = "flat_set",
    hdrs = ["flat_set.h"],
    visibility = ["//visibility:public"],
    deps = [":flat_tree"],
)

base_cc_library(
    name = "stack_container",
    hdrs = ["stack_container.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//base:build_config",
    ],
)
