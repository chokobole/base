load("//bazel:base_cc.bzl", "base_cc_library", "base_cc_test")

base_cc_library(
    name = "auto_reset",
    hdrs = ["auto_reset.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "export",
    hdrs = ["export.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "logging",
    hdrs = ["logging.h"],
    visibility = ["//visibility:public"],
    deps = ["@com_github_google_glog//:glog"],
)

base_cc_library(
    name = "no_destructor",
    hdrs = ["no_destructor.h"],
    visibility = ["//visibility:public"],
)

base_cc_test(
    name = "base_unittests",
    srcs = [
        "auto_reset_unittest.cc",
    ],
    deps = [
        ":auto_reset",
        "@com_google_googletest//:gtest_main",
    ],
)
