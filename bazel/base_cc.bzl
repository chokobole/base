load("@com_chokobole_bazel_utils//:conditions.bzl", "if_windows")
load("@com_chokobole_bazel_utils//:cxxopts.bzl", "cxx14")
load("@com_chokobole_bazel_utils//:dsym.bzl", "dsym")
load("@com_chokobole_bazel_utils//:local_defines_win.bzl", "lean_and_mean")

def base_copts():
    return []

def base_cxxopts():
    return base_copts() + cxx14()

def base_local_defines():
    return if_windows(lean_and_mean())

def base_c_library(
        name,
        copts = base_copts(),
        local_defines = base_local_defines(),
        **kwargs):
    native.cc_library(
        name = name,
        copts = copts,
        local_defines = local_defines,
        **kwargs
    )

def base_cc_library(
        name,
        copts = base_cxxopts(),
        local_defines = base_local_defines(),
        **kwargs):
    native.cc_library(
        name = name,
        copts = copts,
        local_defines = local_defines,
        **kwargs
    )

def base_objc_library(
        name,
        copts = base_cxxopts(),
        tags = ["objc"],
        **kwargs):
    native.objc_library(
        name = name,
        copts = copts,
        tags = tags,
        **kwargs
    )

def base_cc_binary(
        name,
        copts = base_cxxopts(),
        local_defines = base_local_defines(),
        **kwargs):
    native.cc_binary(
        name = name,
        copts = copts,
        local_defines = local_defines,
        **kwargs
    )

    dsym(name = name)

def base_cc_test(
        name,
        copts = base_cxxopts(),
        local_defines = base_local_defines(),
        **kwargs):
    native.cc_test(
        name = name,
        copts = copts,
        local_defines = local_defines,
        **kwargs
    )

    dsym(
        name = name,
        testonly = 1,
    )
