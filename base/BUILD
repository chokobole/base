load("//bazel:base_cc.bzl", "base_cc_library", "base_cc_test")

base_cc_library(
    name = "auto_reset",
    hdrs = ["auto_reset.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "build_config",
    hdrs = ["build_config.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "callback",
    hdrs = ["callback.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "compiler_specific",
    hdrs = ["compiler_specific.h"],
    visibility = ["//visibility:public"],
    deps = [":build_config"],
)

base_cc_library(
    name = "completion_once_callback",
    hdrs = ["completion_once_callback.h"],
    visibility = ["//visibility:public"],
    deps = [":callback"],
)

base_cc_library(
    name = "data_view",
    srcs = ["data_view.cc"],
    hdrs = ["data_view.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":export",
        ":sys_byteorder",
    ],
)

base_cc_library(
    name = "export",
    hdrs = ["export.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "free_deleter",
    hdrs = ["free_deleter.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "logging",
    hdrs = ["logging.h"],
    visibility = ["//visibility:public"],
    deps = ["@com_github_google_glog//:glog"],
)

base_cc_library(
    name = "io_buffer",
    srcs = ["io_buffer.cc"],
    hdrs = ["io_buffer.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":export",
        ":free_deleter",
        ":logging",
    ],
)

base_cc_library(
    name = "macros",
    hdrs = ["macros.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "no_destructor",
    hdrs = ["no_destructor.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "stl_util",
    hdrs = ["stl_util.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":template_util",
        "@com_google_absl//absl/types:optional",
    ],
)

base_cc_library(
    name = "system_error_code",
    srcs = ["system_error_code.cc"],
    hdrs = ["system_error_code.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":build_config",
        ":export",
        ":logging",
        ":stl_util",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

base_cc_library(
    name = "sys_byteorder",
    hdrs = ["sys_byteorder.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":build_config",
        ":logging",
    ],
)

base_cc_library(
    name = "template_util",
    hdrs = ["template_util.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":build_config",
    ],
)

base_cc_test(
    name = "base_unittests",
    srcs = [
        "auto_reset_unittest.cc",
        "data_view_unittest.cc",
        "sys_byteorder_unittest.cc",
    ],
    deps = [
        ":auto_reset",
        ":data_view",
        ":sys_byteorder",
        "@com_google_googletest//:gtest_main",
    ],
)
