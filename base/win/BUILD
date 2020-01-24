load("//bazel:base_cc.bzl", "base_cc_library")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_windows")

base_cc_library(
    name = "scoped_handle",
    srcs = if_windows(["scoped_handle.cc"]),
    hdrs = ["scoped_handle.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":windows_types",
        "//base:compiler_specific",
        "//base:export",
        "//base:logging",
    ],
)

base_cc_library(
    name = "windows_types",
    hdrs = ["windows_types.h"],
    visibility = ["//visibility:public"],
)
