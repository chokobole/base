// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/socket/transport_client_socket.h"

namespace base {

TransportClientSocket::TransportClientSocket() = default;
TransportClientSocket::~TransportClientSocket() = default;

bool TransportClientSocket::SetNoDelay(bool no_delay) {
  NOTIMPLEMENTED();
  return false;
}

bool TransportClientSocket::SetKeepAlive(bool enable, int delay_secs) {
  NOTIMPLEMENTED();
  return false;
}

}  // namespace base
