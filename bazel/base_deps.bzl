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
        )

    if not native.existing_rule("com_chokobole_bazel_utils"):
        http_archive(
            name = "com_chokobole_bazel_utils",
            strip_prefix = "bazel_utils-bb3362a6bff2547b8232e8e68cd6b05279b31a18",
            sha256 = "538d8802b3e17d8e11b3c9096962c95698952cc0baa4087359a5c37cb08f8ecc",
            urls = [
                "https://github.com/chokobole/bazel_utils/archive/bb3362a6bff2547b8232e8e68cd6b05279b31a18.tar.gz",
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
