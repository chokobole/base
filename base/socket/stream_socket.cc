// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/socket/stream_socket.h"

#include "base/logging.h"
#include "base/socket/ip_endpoint.h"
#include "base/socket/socket_errors.h"

namespace base {

void StreamSocket::SetBeforeConnectCallback(
    const BeforeConnectCallback& before_connect_callback) {
  NOTREACHED();
}

StreamSocket::SocketMemoryStats::SocketMemoryStats()
    : total_size(0), buffer_size(0), cert_count(0), cert_size(0) {}

StreamSocket::SocketMemoryStats::~SocketMemoryStats() = default;

int StreamSocket::ConfirmHandshake(CompletionOnceCallback callback) {
  return OK;
}

}  // namespace base
