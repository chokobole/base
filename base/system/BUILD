load("//bazel:base_cc.bzl", "base_cc_library", "base_objc_library")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_mac", "if_posix")

SYS_INFO_HDRS = ["sys_info.h"]

base_cc_library(
    name = "sys_info",
    srcs = ["sys_info.cc"] + select({
        "@com_chokobole_bazel_utils//:linux": ["sys_info_linux.cc"],
        "@com_chokobole_bazel_utils//:windows": ["sys_info_win.cc"],
        "//conditions:default": [],
    }) + if_posix([
        "sys_info_posix.cc",
    ]),
    hdrs = SYS_INFO_HDRS,
    visibility = ["//visibility:public"],
    deps = [
        "//base:no_destructor",
        "//base/files:file_path",
        "//base/files:file_util",
        "//base/posix:eintr_wrapper",
        "//base/process:process_metrics",
    ] + if_mac([":sys_info_mac"]),
)

base_objc_library(
    name = "sys_info_mac",
    srcs = if_mac(["sys_info_mac.mm"]),
    hdrs = SYS_INFO_HDRS,
    deps = [
        "//base:stl_util",
        "//base/mac:scoped_mach_port",
        "//base/process:process_metrics",
        "@com_google_absl//absl/strings:str_format",
    ],
)
