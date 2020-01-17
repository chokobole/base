// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Winsock initialization must happen before any Winsock calls are made.  The
// EnsureWinsockInit method will make sure that WSAStartup has been called.

#ifndef BASE_SOCKET_WINSOCK_INIT_H_
#define BASE_SOCKET_WINSOCK_INIT_H_

#include "base/export.h"

namespace base {

// Make sure that Winsock is initialized, calling WSAStartup if needed.
BASE_EXPORT void EnsureWinsockInit();

}  // namespace base

#endif  // BASE_SOCKET_WINSOCK_INIT_H_
