load("//bazel:base_cc.bzl", "base_cc_library")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_windows")

base_cc_library(
    name = "can_lower_nice_to",
    srcs = select({
        "@com_chokobole_bazel_utils//:mac_or_ios": [],
        "@com_chokobole_bazel_utils//:windows": [],
        "//conditions:default": ["can_lower_nice_to.cc"],
    }),
    hdrs = ["can_lower_nice_to.h"],
    visibility = ["//visibility:public"],
    deps = ["//base:build_config"],
)

base_cc_library(
    name = "eintr_wrapper",
    hdrs = ["eintr_wrapper.h"],
    visibility = ["//visibility:public"],
    deps = ["//base:build_config"],
)
