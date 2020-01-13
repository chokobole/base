load("//bazel:base_cc.bzl", "base_cc_library")

base_cc_library(
    name = "event_loop",
    srcs = [
        "event_loop.cc",
    ],
    hdrs = [
        "event_loop.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//base:auto_reset",
        "//base:export",
        "//base:logging",
        "@com_github_libevent_libevent//:libevent",
    ],
)
