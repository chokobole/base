load("//bazel:base_cc.bzl", "base_cc_library")
load("@com_chokobole_bazel_utils//:conditions.bzl", "if_mac", "if_windows")

base_cc_library(
    name = "free_deleter",
    hdrs = ["free_deleter.h"],
    visibility = ["//visibility:public"],
)

base_cc_library(
    name = "shared_memory",
    srcs = [
        "platform_shared_memory_region.cc",
        "read_only_shared_memory_region.cc",
        "shared_memory_mapping.cc",
        "unsafe_shared_memory_region.cc",
        "writable_shared_memory_region.cc",
    ] + select({
        "@com_chokobole_bazel_utils//:mac": ["platform_shared_memory_region_mac.cc"],
        "@com_chokobole_bazel_utils//:windows": ["platform_shared_memory_region_win.cc"],
        "//conditions:default": ["platform_shared_memory_region_posix.cc"],
    }),
    hdrs = [
        "platform_shared_memory_region.h",
        "read_only_shared_memory_region.h",
        "shared_memory_mapping.h",
        "unsafe_shared_memory_region.h",
        "writable_shared_memory_region.h",
    ],
    linkopts = if_windows([
        "advapi32.lib",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//base:build_config",
        "//base:logging",
        "//base:unguessable_token",
        "//base/numerics:checked_math",
        "@com_google_absl//absl/types:span",
    ] + select({
        "@com_chokobole_bazel_utils//:mac": [
            "//base/mac:mach_logging",
            "//base/mac:scoped_mach_port",
            "//base/mac:scoped_mach_vm",
        ],
        "@com_chokobole_bazel_utils//:windows": [
            "//base:bits",
            "//base/process:process_handle",
            "//base/strings",
            "//base/win:scoped_handle",
            "//base/win:windows_types",
        ],
        "//conditions:default": [
            "//base:file_descriptor_posix",
            "//base/files:file_util",
        ],
    }),
)
