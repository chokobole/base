# Base

A C++ base library.

## Contents
- [Base](#base)
  - [Contents](#contents)
  - [How to use](#how-to-use)

## How to use

To use `base`, add the followings to your `WORKSPACE` file.

```python
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# If you want to use a specific version, use like below.
http_archive(
    name = "com_chokobole_base",
    sha256 = "<sha256>",
    strip_prefix = "base-<commit>",
    urls = [
        "https://github.com/chokobole/base/archive/<commit>.tar.gz",
    ],
)

# Or if you just try, use like below.
http_archive(
    name = "com_chokobole_base",
    strip_prefix = "base-master",
    urls = [
        "https://github.com/chokobole/base/archive/master.tar.gz",
    ],
)

load("@com_chokobole_base//bazel:base_deps.bzl", "base_deps")

base_deps()
```
