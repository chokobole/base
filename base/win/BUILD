load("//bazel:base_cc.bzl", "base_cc_library")

base_cc_library(
    name = "scoped_handle",
    hdrs = ["scoped_handle.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":windows_types",
        "//base:compiler_specific",
        "//base:logging",
    ],
)

base_cc_library(
    name = "windows_types",
    hdrs = ["windows_types.h"],
    visibility = ["//visibility:public"],
)
