load("//bazel:base_cc.bzl", "base_cc_library")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_windows")

base_cc_library(
    name = "thread_local",
    srcs = if_windows(
        [
            "thread_local_storage_win.cc",
        ],
        [
            "thread_local_storage_posix.cc",
        ],
    ),
    hdrs = [
        "thread_local.h",
        "thread_local_storage.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//base:build_config",
        "//base:export",
        "//base:logging",
    ],
)
