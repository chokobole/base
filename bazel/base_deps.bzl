load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def base_deps():
    if not native.existing_rule("com_google_absl"):
        http_archive(
            name = "com_google_absl",
            strip_prefix = "abseil-cpp-aa844899c937bde5d2b24f276b59997e5b668bde",
            sha256 = "f1a959a2144f0482b9bd61e67a9897df02234fff6edf82294579a4276f2f4b97",
            urls = [
                "https://github.com/abseil/abseil-cpp/archive/aa844899c937bde5d2b24f276b59997e5b668bde.zip",  # 20190808
            ],
            patch_args = ["-p1"],
            patches = ["@com_chokobole_base//third_party:abseil-cpp.patch"],
        )

    if not native.existing_rule("com_chokobole_bazel_utils"):
        http_archive(
            name = "com_chokobole_bazel_utils",
            strip_prefix = "bazel_utils-a4a6c24f630d153fb99f26b5789eb02a661220f2",
            sha256 = "a3da7898bc587849a7503932d04fb2f7dc6c189d469c62e0af7069c678380dd5",
            urls = [
                "https://github.com/chokobole/bazel_utils/archive/a4a6c24f630d153fb99f26b5789eb02a661220f2.tar.gz",
            ],
        )

    if not native.existing_rule("com_google_googletest"):
        http_archive(
            name = "com_google_googletest",
            sha256 = "353ab86e35cea1cd386115279cf4b16695bbf21b897bfbf2721cf4cb5f64ade8",
            strip_prefix = "googletest-997d343dd680e541ef96ce71ee54a91daf2577a0",
            urls = [
                "https://mirror.bazel.build/github.com/google/googletest/archive/997d343dd680e541ef96ce71ee54a91daf2577a0.zip",
                "https://github.com/google/googletest/archive/997d343dd680e541ef96ce71ee54a91daf2577a0.zip",
            ],
        )

    # Needed by com_github_google_glog
    if not native.existing_rule("com_github_gflags_gflags"):
        http_archive(
            name = "com_github_gflags_gflags",
            strip_prefix = "gflags-2.2.2",
            urls = [
                "https://mirror.bazel.build/github.com/gflags/gflags/archive/v2.2.2.tar.gz",
                "https://github.com/gflags/gflags/archive/v2.2.2.tar.gz",
            ],
        )

    if not native.existing_rule("com_github_google_glog"):
        http_archive(
            name = "com_github_google_glog",
            sha256 = "ba51f089471fa4084e1d073dfb4a0fc81679f1d016faa58aefde5738345fee93",
            strip_prefix = "glog-130a3e10de248344cdaeda54aed4c8a5ad7cedac",
            urls = [
                "https://github.com/google/glog/archive/130a3e10de248344cdaeda54aed4c8a5ad7cedac.zip",
            ],
        )

    if not native.existing_rule("com_github_libevent_libevent"):
        http_archive(
            name = "com_github_libevent_libevent",
            sha256 = "dffa4e78139a6f927edc6396c9c54d1aa4dbf8413e537863c59b179d7beabdd0",
            strip_prefix = "libevent-release-2.1.11-stable",
            urls = [
                "https://github.com/libevent/libevent/archive/release-2.1.11-stable.zip",
            ],
            build_file = "@//third_party/libevent:libevent.BUILD",
        )
