load("@com_chokobole_bazel_utils//:conditions.bzl", "if_windows")

cc_binary(
    name = "unix_domain_socket",
    srcs = if_windows(
        ["//examples:only_main.cc"],
        ["unix_domain_socket.cc"],
    ),
    deps = ["//base/socket:unix_domain_socket"],
)
