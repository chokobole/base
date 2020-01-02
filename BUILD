load("@com_github_bazelbuild_buildtools//buildifier:def.bzl", "buildifier")

exports_files(["LICENSE"])

package_group(
    name = "internal",
    packages = [
        "//base/...",
    ],
)

buildifier(
    name = "buildifier",
)
