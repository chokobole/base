load("//bazel:base_cc.bzl", "base_cc_library", "base_cc_test")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_posix", "if_windows")

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
    name = "datagram_buffer",
    srcs = ["datagram_buffer.cc"],
    hdrs = ["datagram_buffer.h"],
    deps = [
        "//base:export",
        "//base:logging",
        "@com_google_absl//absl/memory",
    ],
)

base_cc_library(
    name = "datagram_socket",
    hdrs = [
        "datagram_client_socket.h",
        "datagram_server_socket.h",
        "datagram_socket.h",
    ],
    deps = [
        ":datagram_buffer",
        ":diff_serv_code_point",
        ":socket",
    ],
)

base_cc_library(
    name = "diff_serv_code_point",
    hdrs = ["diff_serv_code_point.h"],
)

base_cc_library(
    name = "ip_endpoint",
    srcs = ["ip_endpoint.cc"],
    hdrs = ["ip_endpoint.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":address_family",
        ":winsock_util",
        "//base:sys_byteorder",
    ],
)

base_cc_library(
    name = "socket_posix",
    srcs = if_posix([
        "socket_posix.cc",
    ]),
    hdrs = if_posix([
        "socket_posix.h",
    ]),
    deps = [
        ":ip_endpoint",
        ":sockaddr_storage",
        ":socket_descriptor",
        ":socket_errors",
        "//base:completion_once_callback",
        "//base:io_buffer",
        "//base/event_loop",
        "//base/files:file_util",
        "@com_google_absl//absl/functional:bind_front",
    ],
)

base_cc_library(
    name = "server_socket",
    srcs = ["server_socket.cc"],
    hdrs = ["server_socket.h"],
    deps = [
        ":ip_endpoint",
        ":socket_errors",
        "//base:completion_once_callback",
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
    name = "socket",
    srcs = ["socket.cc"],
    hdrs = ["socket.h"],
    deps = [
        ":socket_errors",
        "//base:completion_once_callback",
    ],
)

base_cc_library(
    name = "socket_descriptor",
    srcs = ["socket_descriptor.cc"],
    hdrs = ["socket_descriptor.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":winsock_init",
        "//base:build_config",
        "//base:export",
    ],
)

base_cc_library(
    name = "socket_errors",
    srcs = ["socket_errors.cc"] + if_windows([
        "socket_errors_win.cc",
    ]) + if_posix([
        "socket_errors_posix.cc",
    ]),
    hdrs = [
        "socket_error_list.h",
        "socket_errors.h",
    ],
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
    name = "stream_socket",
    srcs = ["stream_socket.cc"],
    hdrs = ["stream_socket.h"],
    deps = [
        ":ip_endpoint",
        ":socket",
        ":socket_errors",
        "//base:completion_once_callback",
    ],
)

base_cc_library(
    name = "sys_addrinfo",
    hdrs = ["sys_addrinfo.h"],
    visibility = ["//visibility:public"],
    deps = ["//base:build_config"],
)

base_cc_library(
    name = "tcp_socket",
    srcs = if_posix([
        "tcp_client_socket.cc",
        "tcp_server_socket.cc",
        "tcp_socket_posix.cc",
    ]),
    hdrs = if_posix([
        "tcp_client_socket.h",
        "tcp_server_socket.h",
        "tcp_socket.h",
        "tcp_socket_posix.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":address_list",
        ":server_socket",
        ":socket_options",
        ":socket_posix",
        ":transport_client_socket",
        "@com_google_absl//absl/functional:bind_front",
        "@com_google_absl//absl/time",
    ],
)

base_cc_library(
    name = "transport_client_socket",
    srcs = ["transport_client_socket.cc"],
    hdrs = ["transport_client_socket.h"],
    deps = [":stream_socket"],
)

base_cc_library(
    name = "udp_socket",
    srcs = if_posix([
        "udp_client_socket.cc",
        "udp_server_socket.cc",
        "udp_socket_posix.cc",
    ]),
    hdrs = if_posix([
        "udp_client_socket.h",
        "udp_server_socket.h",
        "udp_socket.h",
        "udp_socket_posix.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":datagram_socket",
        ":socket_options",
        ":socket_posix",
        "@com_google_absl//absl/random",
        "@com_google_absl//absl/time",
    ],
)

base_cc_library(
    name = "unix_domain_socket",
    srcs = if_posix([
        "unix_domain_client_socket_posix.cc",
        "unix_domain_server_socket_posix.cc",
    ]),
    hdrs = if_posix([
        "unix_domain_client_socket_posix.h",
        "unix_domain_server_socket_posix.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":server_socket",
        ":socket_posix",
        ":stream_socket",
        "@com_google_absl//absl/functional:bind_front",
    ],
)

base_cc_library(
    name = "winsock_init",
    srcs = if_windows(["winsock_init.cc"]),
    hdrs = if_windows(["winsock_init.h"]),
    deps = [
        "//base:export",
        "//base:logging",
        "@com_google_absl//absl/base",
    ],
)

base_cc_library(
    name = "winsock_util",
    srcs = if_windows(["winsock_util.cc"]),
    hdrs = if_windows(["winsock_util.h"]),
    deps = [
        ":socket_errors",
        "//base:logging",
    ],
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
