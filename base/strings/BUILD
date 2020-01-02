load("//bazel:base_cc.bzl", "base_cc_library", "base_cc_test")

base_cc_library(
    name = "strings",
    srcs = [
        "string_number_conversions.cc",
        "string_util.cc",
    ],
    hdrs = [
        "string_number_conversions.h",
        "string_util.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//base:export",
        "//base:no_destructor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

base_cc_test(
    name = "strings_unittests",
    srcs = [
        "string_util_unittest.cc",
    ],
    deps = [
        ":strings",
        "@com_google_googletest//:gtest_main",
    ],
)
