load("//bazel:base_cc.bzl", "base_cc_library", "base_cc_test")

package_group(
    name = "internal",
    packages = [
        "//base/...",
    ],
)

base_cc_library(
    name = "auto_reset",
    hdrs = ["auto_reset.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "bits",
    hdrs = ["bits.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "build_config",
    hdrs = ["build_config.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "callback",
    hdrs = [
        "callback.h",
        "callback_forward.h",
    ],
    visibility = ["//visibility:public"],
    deps = ["@com_google_absl//absl/functional:function_ref"],
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
    name = "environment",
    srcs = ["environment.cc"],
    hdrs = ["environment.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":build_config",
        ":export",
        "@com_google_absl//absl/strings",
    ],
)

base_cc_library(
    name = "export",
    hdrs = ["export.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "file_descriptor_posix",
    hdrs = ["file_descriptor_posix.h"],
    visibility = ["//visibility:public"],
    deps = ["//base/files:file"],
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
        ":logging",
        "//base/memory:free_deleter",
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
    name = "scoped_generic",
    hdrs = ["scoped_generic.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":compiler_specific",
        ":logging",
    ],
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

base_cc_library(
    name = "token",
    srcs = ["token.cc"],
    hdrs = ["token.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":export",
        "@com_google_absl//absl/hash",
        "@com_google_absl//absl/random",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/types:optional",
    ],
)

base_cc_library(
    name = "unguessable_token",
    srcs = ["unguessable_token.cc"],
    hdrs = ["unguessable_token.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":logging",
        ":no_destructor",
        ":token",
    ],
)

base_cc_test(
    name = "base_unittests",
    srcs = [
        "auto_reset_unittest.cc",
        "bits_unittest.cc",
        "data_view_unittest.cc",
        "environment_unittest.cc",
        "scoped_generic_unittest.cc",
        "sys_byteorder_unittest.cc",
    ],
    deps = [
        ":auto_reset",
        ":bits",
        ":data_view",
        ":environment",
        ":scoped_generic",
        ":stl_util",
        ":sys_byteorder",
        "@com_google_googletest//:gtest_main",
    ],
)
