// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_TCP_SOCKET_H_
#define BASE_SOCKET_TCP_SOCKET_H_

#include "base/build_config.h"
#include "base/export.h"
#include "base/socket/socket_descriptor.h"

#if defined(OS_WIN)
// #include "base/socket/tcp_socket_win.h"
#elif defined(OS_POSIX)
#include "base/socket/tcp_socket_posix.h"
#endif

namespace base {

// TCPSocket provides a platform-independent interface for TCP sockets.
//
// It is recommended to use TCPClientSocket/TCPServerSocket instead of this
// class, unless a clear separation of client and server socket functionality is
// not suitable for your use case (e.g., a socket needs to be created and bound
// before you know whether it is a client or server socket).
#if defined(OS_WIN)
// typedef TCPSocketWin TCPSocket;
#elif defined(OS_POSIX)
typedef TCPSocketPosix TCPSocket;
#endif

}  // namespace base

#endif  // BASE_SOCKET_TCP_SOCKET_H_
