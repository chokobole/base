load("//bazel:base_cc.bzl", "base_cc_library")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_linux", "if_windows")

base_cc_library(
    name = "process_handle",
    srcs = ["process_handle.cc"] + select({
        "@com_chokobole_bazel_utils//:freebsd": ["process_handle_freebsd.cc"],
        "@com_chokobole_bazel_utils//:linux": ["process_handle_linux.cc"],
        "@com_chokobole_bazel_utils//:mac": ["process_handle_mac.cc"],
        "@com_chokobole_bazel_utils//:windows": ["process_handle_win.cc"],
        "//conditions:default": [],
    }) + if_windows(
        [],
        [
            "process_handle_posix.cc",
        ],
    ),
    hdrs = ["process_handle.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//base/files:file_path",
    ] + select({
        "@com_chokobole_bazel_utils//:freebsd": ["//base:stl_util"],
        "@com_chokobole_bazel_utils//:linux": [":internal_linux"],
        "@com_chokobole_bazel_utils//:mac": ["//base:logging"],
        "@com_chokobole_bazel_utils//:windows": [
            "//base/win:scoped_handle",
            "//base/win:windows_types",
        ],
        "//conditions:default": [],
    }),
)

base_cc_library(
    name = "internal_linux",
    srcs = if_linux(["internal_linux.cc"]),
    hdrs = ["internal_linux.h"],
    visibility = ["//base:internal"],
    deps = [
        "//base/files:file_util",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
    ],
)
