// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/socket/socket.h"

#include "base/socket/socket_errors.h"

namespace base {

Socket::~Socket() = default;

int Socket::ReadIfReady(std::shared_ptr<IOBuffer> buf, int buf_len,
                        CompletionOnceCallback callback) {
  return ERR_READ_IF_READY_NOT_IMPLEMENTED;
}

int Socket::CancelReadIfReady() { return ERR_READ_IF_READY_NOT_IMPLEMENTED; }

}  // namespace base
