licenses(["notice"])  # BSD-3-Clause

exports_files(["LICENSE"])

include_files = [
    "libevent/include/evdns.h",
    "libevent/include/event.h",
    "libevent/include/evhttp.h",
    "libevent/include/evrpc.h",
    "libevent/include/evutil.h",
    "libevent/include/event2/buffer.h",
    "libevent/include/event2/bufferevent_struct.h",
    "libevent/include/event2/event.h",
    "libevent/include/event2/http_struct.h",
    "libevent/include/event2/rpc_struct.h",
    "libevent/include/event2/buffer_compat.h",
    "libevent/include/event2/dns.h",
    "libevent/include/event2/event_compat.h",
    "libevent/include/event2/keyvalq_struct.h",
    "libevent/include/event2/tag.h",
    "libevent/include/event2/bufferevent.h",
    "libevent/include/event2/dns_compat.h",
    "libevent/include/event2/event_struct.h",
    "libevent/include/event2/listener.h",
    "libevent/include/event2/tag_compat.h",
    "libevent/include/event2/bufferevent_compat.h",
    "libevent/include/event2/dns_struct.h",
    "libevent/include/event2/http.h",
    "libevent/include/event2/rpc.h",
    "libevent/include/event2/thread.h",
    "libevent/include/event2/event-config.h",
    "libevent/include/event2/http_compat.h",
    "libevent/include/event2/rpc_compat.h",
    "libevent/include/event2/util.h",
    "libevent/include/event2/visibility.h",
]

lib_files = [
    "libevent/lib/libevent.a",
    "libevent/lib/libevent_core.a",
    "libevent/lib/libevent_extra.a",
    "libevent/lib/libevent_pthreads.a",
    "libevent/lib/event.lib",
    "libevent/lib/event_core.lib",
    "libevent/lib/event_extra.lib",
]

genrule(
    name = "libevent-srcs",
    srcs = glob([
        "**/*",
    ]),
    outs = include_files + lib_files,
    cmd = select({
        "@com_chokobole_bazel_utils//:windows": "\n".join([
            "export INSTALL_DIR=$$(pwd)/$(@D)/libevent",
            "export TMP_DIR=$$(mktemp -d -t libevent.XXXXXX)",
            "mkdir -p $$TMP_DIR",
            "cp -R $$(pwd)/external/com_github_libevent_libevent/* $$TMP_DIR",
            "cd $$TMP_DIR",
            "mkdir build && cd build",
            "cmake " +
            "-DCMAKE_INSTALL_PREFIX=$$INSTALL_DIR " +
            "-DCMAKE_GENERATOR_PLATFORM=x64 " +
            "-DCMAKE_CL_64=1" +
            "-Ax64 " +
            "-DEVENT__DISABLE_OPENSSL=ON " +
            "-DEVENT__DISABLE_THREAD_SUPPORT=OFF " +
            "-DEVENT__LIBRARY_TYPE=STATIC " +
            "..",
            "cmake --build . --config Release",
            "cmake --install .",
            "rm -rf $$TMP_DIR build",
            "touch $$INSTALL_DIR/lib/libevent.a",
            "touch $$INSTALL_DIR/lib/libevent_core.a",
            "touch $$INSTALL_DIR/lib/libevent_extra.a",
            "touch $$INSTALL_DIR/lib/libevent_pthreads.a",
        ]),
        "//conditions:default": "\n".join([
            "export INSTALL_DIR=$$(pwd)/$(@D)/libevent",
            "export TMP_DIR=$$(mktemp -d -t libevent.XXXXXX)",
            "mkdir -p $$TMP_DIR",
            "cp -R $$(pwd)/external/com_github_libevent_libevent/* $$TMP_DIR",
            "cd $$TMP_DIR",
            "mkdir build && cd build",
            "cmake " +
            "-DCMAKE_INSTALL_PREFIX=$$INSTALL_DIR " +
            "-DCMAKE_C_FLAGS=-fPIC " +
            "-DCMAKE_CXX_FLAGS=-fPIC " +
            "-DEVENT__DISABLE_OPENSSL=ON " +
            "-DEVENT__DISABLE_THREAD_SUPPORT=OFF " +
            "-DEVENT__LIBRARY_TYPE=STATIC " +
            "..",
            "make -j",
            "make install",
            "rm -rf $$TMP_DIR build",
            "touch $$INSTALL_DIR/lib/event.lib",
            "touch $$INSTALL_DIR/lib/event_core.lib",
            "touch $$INSTALL_DIR/lib/event_extra.lib",
        ]),
    }),
)

cc_library(
    name = "libevent",
    srcs = select({
        "@com_chokobole_bazel_utils//:windows": [
            "libevent/lib/event.lib",
        ],
        "//conditions:default": [
            "libevent/lib/libevent.a",
            "libevent/lib/libevent_pthreads.a",
        ],
    }),
    hdrs = include_files,
    includes = ["libevent/include"],
    linkopts = select({
        "@com_chokobole_bazel_utils//:windows": [
            "advapi32.lib",
            "ws2_32.lib",
        ],
        "//conditions:default": ["-lpthread"],
    }),
    linkstatic = 1,
    visibility = ["//visibility:public"],
)

filegroup(
    name = "libevent-files",
    srcs = include_files + lib_files,
    visibility = ["//visibility:public"],
)
