load("//bazel:base_cc.bzl", "base_cc_library", "base_cc_test")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_windows")

base_cc_library(
    name = "address_family",
    srcs = ["address_family.cc"],
    hdrs = ["address_family.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":ip_address",
        ":sys_addrinfo",
    ],
)

base_cc_library(
    name = "ip_address",
    srcs = ["ip_address.cc"],
    hdrs = ["ip_address.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//base:compiler_specific",
        "//base:export",
        "//base:logging",
        "//base:stl_util",
        "//base/containers:stack_container",
        "//base/strings",
    ],
)

base_cc_library(
    name = "ip_endpoint",
    srcs = ["ip_endpoint.cc"],
    hdrs = ["ip_endpoint.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":address_family",
        "//base:sys_byteorder",
    ],
)

base_cc_library(
    name = "socket_descriptor",
    hdrs = ["socket_descriptor.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//base:build_config",
        "//base:export",
    ],
)

base_cc_library(
    name = "sockaddr_storage",
    srcs = ["sockaddr_storage.cc"],
    hdrs = ["sockaddr_storage.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//base:build_config",
        "//base:export",
    ],
)

base_cc_library(
    name = "socket_errors",
    srcs = if_windows(
        [
            "socket_errors_win.cc",
        ],
        [
            "socket_errors_posix.cc",
        ],
    ),
    hdrs = ["socket_errors.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//base:build_config",
        "//base:export",
        "//base:logging",
        "//base:system_error_code",
    ],
)

base_cc_library(
    name = "sys_addrinfo",
    hdrs = ["sys_addrinfo.h"],
    visibility = ["//visibility:public"],
    deps = ["//base:build_config"],
)

base_cc_test(
    name = "socket_unittests",
    srcs = [
        "ip_address_unittest.cc",
        "ip_endpoint_unittest.cc",
    ],
    deps = [
        ":ip_endpoint",
        ":sockaddr_storage",
        "@com_google_googletest//:gtest_main",
    ],
)
