load("//bazel:base_cc.bzl", "base_cc_library")

base_cc_library(
    name = "time_util",
    srcs = ["time_util.cc"],
    hdrs = ["time_util.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//base:build_config",
        "//base:export",
        "//base:logging",
        "//base/win:windows_types",
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/time",
    ],
)
