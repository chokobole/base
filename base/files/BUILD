load("//bazel:base_cc.bzl", "base_cc_library")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_windows")

base_cc_library(
    name = "file_util",
    srcs = if_windows(
        [
            "file_util_win.cc",
        ],
        [
            "file_util_posix.cc",
        ],
    ),
    hdrs = ["file_util.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//base:export",
        "//base/posix:eintr_wrapper",
    ],
)
