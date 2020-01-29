load("//bazel:base_cc.bzl", "base_cc_library")

base_cc_library(
    name = "checked_math",
    hdrs = [
        "checked_math.h",
        "checked_math_impl.h",
    ],
    visibility = ["//visibility:public"],
    deps = [":safe_math_impl"],
)

base_cc_library(
    name = "safe_conversions",
    hdrs = [
        "safe_conversions.h",
        "safe_conversions_arm_impl.h",
        "safe_conversions_impl.h",
    ],
    visibility = ["//visibility:public"],
    deps = ["//base:build_config"],
)

base_cc_library(
    name = "safe_math_impl",
    hdrs = [
        "safe_math_arm_impl.h",
        "safe_math_clang_gcc_impl.h",
        "safe_math_shared_impl.h",
    ],
    deps = [":safe_conversions"],
)
