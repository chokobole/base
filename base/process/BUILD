load("//bazel:base_cc.bzl", "base_cc_library")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_linux", "if_mac", "if_windows")

base_cc_library(
    name = "port_provider_mac",
    hdrs = ["port_provider_mac.h"],
    deps = [":process_handle"],
)

# TODO: Should deal with process_fuchsia.cc
base_cc_library(
    name = "process",
    srcs = select({
        "@com_chokobole_bazel_utils//:linux": [
            "process_posix.cc",
            "process_linux.cc",
        ],
        "@com_chokobole_bazel_utils//:mac": [
            "process_posix.cc",
            "process_mac.cc",
        ],
        "@com_chokobole_bazel_utils//:windows": ["process_win.cc"],
        "//conditions:default": ["process_posix.cc"],
    }),
    hdrs = ["process.h"],
    deps = [
        ":process_handle",
        "@com_google_absl//absl/time",
    ] + if_mac([
        ":port_provider_mac",
        "//base/mac:mach_logging",
        "//base/memory:free_deleter",
    ]) + if_windows(
        [
            "//base/numerics:safe_conversions",
            "//base/time:time_util",
        ],
        [
            "//base/files:scoped_file",
            "//base/posix:can_lower_nice_to",
        ],
    ),
)

# TODO: Should deal with process_handle_fuchsia.cc and process_handle_openbsd.cc
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
