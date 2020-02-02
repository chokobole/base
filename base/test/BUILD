load("//bazel:base_cc.bzl", "base_cc_library")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_posix")

base_cc_library(
    name = "scoped_locale",
    testonly = 1,
    srcs = if_posix(["scoped_locale.cc"]),
    hdrs = if_posix(["scoped_locale.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_googletest//:gtest",
    ],
)
