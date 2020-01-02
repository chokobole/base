load("@com_chokobole_bazel_utils//:cxxopts.bzl", "cxx14")
load("@com_chokobole_bazel_utils//:dsym.bzl", "dsym")

def base_copts():
    return cxx14()

def base_cc_library(
        name,
        copts = base_copts(),
        **kwargs):
    native.cc_library(
        name = name,
        copts = copts,
        **kwargs
    )

def base_cc_binary(
        name,
        copts = base_copts(),
        **kwargs):
    native.cc_binary(
        name = name,
        copts = copts,
        **kwargs
    )

    dsym(name = name)

def base_cc_test(
        name,
        copts = base_copts(),
        **kwargs):
    native.cc_test(
        name = name,
        copts = copts,
        **kwargs
    )

    dsym(
        name = name,
        testonly = 1,
    )
