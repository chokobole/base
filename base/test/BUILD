load("//bazel:base_cc.bzl", "base_cc_library")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_windows")

base_cc_library(
    name = "scoped_locale",
    testonly = 1,
    srcs = if_windows(
        [],
        ["scoped_locale.cc"],
    ),
    hdrs = if_windows(
        [],
        ["scoped_locale.h"],
    ),
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_googletest//:gtest",
    ],
)
