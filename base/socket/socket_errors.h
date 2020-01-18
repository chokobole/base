// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Modified from net/base/net_errors.h

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_ERRORS_H_
#define BASE_SOCKET_ERRORS_H_

#include <string>

#include "base/export.h"
#include "base/system_error_code.h"

namespace base {

// Error values are negative.
enum SocketErrorCode {
  OK = 0,

#define SOCKET_ERROR(label, value) ERR_##label = value,
#include "base/socket/socket_error_list.h"
#undef SOCKET_ERROR
};

// Same as above, but leaves off the leading "net::".
BASE_EXPORT std::string ErrorToShortString(int error);

// Map system error code to Error.
BASE_EXPORT SocketErrorCode MapSystemError(SystemErrorCode os_error);

}  // namespace base

#endif  // BASE_SOCKET_ERRORS_H_