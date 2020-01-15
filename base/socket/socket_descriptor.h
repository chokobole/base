// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_SOCKET_DESCRIPTOR_H_
#define BASE_SOCKET_SOCKET_DESCRIPTOR_H_

#include "base/build_config.h"
#include "base/export.h"

#if defined(OS_WIN)
#include <winsock2.h>
#endif  // OS_WIN

namespace base {

#if defined(OS_WIN)
typedef SOCKET SocketDescriptor;
const SocketDescriptor kInvalidSocket = INVALID_SOCKET;
#elif defined(OS_POSIX)
typedef int SocketDescriptor;
const SocketDescriptor kInvalidSocket = -1;
#endif

// Creates socket. See WSASocket/socket documentation of parameters.
BASE_EXPORT SocketDescriptor CreatePlatformSocket(int family, int type,
                                                  int protocol);

}  // namespace base

#endif  // BASE_SOCKET_SOCKET_DESCRIPTOR_H_
