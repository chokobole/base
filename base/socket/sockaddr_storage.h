// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_SOCKADDR_STORAGE_H_
#define BASE_SOCKET_SOCKADDR_STORAGE_H_

#include "base/build_config.h"

#if defined(OS_WIN)
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(OS_POSIX)
#include <sys/socket.h>
#include <sys/types.h>
#endif

#include "base/export.h"

namespace base {

// Convenience struct for when you need a |struct sockaddr|.
struct BASE_EXPORT SockaddrStorage {
  SockaddrStorage();
  SockaddrStorage(const SockaddrStorage& other);
  void operator=(const SockaddrStorage& other);

  struct sockaddr_storage addr_storage;
  socklen_t addr_len;
  struct sockaddr* const addr;
};

}  // namespace base

#endif  // BASE_SOCKET_SOCKADDR_STORAGE_H_
