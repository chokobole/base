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
    name = "address_list",
    srcs = ["address_list.cc"],
    hdrs = ["address_list.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":ip_endpoint",
        ":sockaddr_storage",
        "//base/containers:flat_map",
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
    name = "platfrom_socket",
    srcs = if_windows(
        [
        ],
        [
            "socket_posix.cc",
        ],
    ),
    hdrs = if_windows(
        [
        ],
        [
            "socket_posix.h",
        ],
    ),
    deps = [
        ":ip_endpoint",
        ":sockaddr_storage",
        ":socket_descriptor",
        ":socket_errors",
        "//base:completion_once_callback",
        "//base:io_buffer",
        "//base/event_loop",
        "//base/files:file_util",
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
    name = "socket_descriptor",
    hdrs = ["socket_descriptor.h"],
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
    name = "socket_options",
    srcs = ["socket_options.cc"],
    hdrs = ["socket_options.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":socket_descriptor",
        ":socket_errors",
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
        "address_list_unittest.cc",
        "ip_address_unittest.cc",
        "ip_endpoint_unittest.cc",
    ],
    deps = [
        ":address_list",
        "@com_google_googletest//:gtest_main",
    ],
)
