load("//bazel:base_cc.bzl", "base_cc_library", "base_cc_test")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_mac")

base_cc_library(
    name = "mach_logging",
    srcs = if_mac(["mach_logging.cc"]),
    hdrs = ["mach_logging.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//base:build_config",
        "//base:export",
        "//base:logging",
        "@com_google_absl//absl/strings:str_format",
    ]
)

base_cc_library(
    name = "scoped_mach_port",
    srcs = if_mac(["scoped_mach_port.cc"]),
    hdrs = ["scoped_mach_port.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":mach_logging",
        "//base:scoped_generic",
        "@com_google_absl//absl/types:optional",
    ],
)

base_cc_library(
    name = "scoped_mach_vm",
    srcs = if_mac(["scoped_mach_vm.cc"]),
    hdrs = ["scoped_mach_vm.h"],
    visibility = ["//visibility:public"],
    deps = [":mach_logging"],
)

base_cc_test(
    name = "mac_unittests",
    srcs = if_mac([
        "scoped_mach_vm_unittest.cc",
    ]),
    deps = [
        ":scoped_mach_vm",
        "@com_google_googletest//:gtest_main",
    ],
)