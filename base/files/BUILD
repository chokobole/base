load("//bazel:base_cc.bzl", "base_cc_library", "base_cc_test")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_android", "if_windows")

base_cc_library(
    name = "file",
    srcs = ["file.cc"] + if_windows(
        [
            "file_win.cc",
        ],
        [
            "file_posix.cc",
        ],
    ),
    hdrs = ["file.h"],
    deps = [
        ":file_path",
        ":platform_file",
        "//base/numerics:safe_conversions",
        "//base/time:time_util",
        "@com_google_absl//absl/types:span",
    ],
)

base_cc_library(
    name = "file_path",
    srcs = [
        "file_path.cc",
        "file_path_constants.cc",
    ],
    hdrs = ["file_path.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//base:build_config",
        "//base:compiler_specific",
        "//base:export",
        "//base:logging",
        "//base:stl_util",
        "@com_google_absl//absl/strings",
    ],
)

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

base_cc_library(
    name = "platform_file",
    hdrs = ["platform_file.h"],
    visibility = ["//visibility:public"],
    deps = [":scoped_file"] + if_windows([
        "//base/win:scoped_handle",
        "//base/win:windows_types",
    ]),
)

base_cc_library(
    name = "scoped_file",
    srcs = ["scoped_file.cc"] + if_android([
        "scoped_file_android.cc",
    ]),
    hdrs = ["scoped_file.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//base:build_config",
        "//base:export",
        "//base:logging",
        "//base:scoped_generic",
        "@com_google_absl//absl/strings",
    ] + if_windows(
        [],
        [
            "//base/posix:eintr_wrapper",
        ],
    ),
)

base_cc_test(
    name = "files_unittests",
    srcs = [
        "file_path_unittest.cc",
    ],
    deps = [
        ":file_path",
        "@com_google_googletest//:gtest_main",
    ],
)
