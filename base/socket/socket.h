// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_SOCKET_H_
#define BASE_SOCKET_SOCKET_H_

#include <stdint.h>

#include <memory>

#include "base/completion_once_callback.h"
#include "base/export.h"

namespace base {

class IOBuffer;

// Represents a read/write socket.
class BASE_EXPORT Socket {
 public:
  virtual ~Socket();

  // Reads data, up to |buf_len| bytes, from the socket.  The number of bytes
  // read is returned, or an error is returned upon failure.
  // ERR_SOCKET_NOT_CONNECTED should be returned if the socket is not currently
  // connected.  Zero is returned once to indicate end-of-file; the return value
  // of subsequent calls is undefined, and may be OS dependent.  ERR_IO_PENDING
  // is returned if the operation could not be completed synchronously, in which
  // case the result will be passed to the callback when available. If the
  // operation is not completed immediately, the socket acquires a reference to
  // the provided buffer until the callback is invoked or the socket is
  // closed.  If the socket is Disconnected before the read completes, the
  // callback will not be invoked.
  virtual int Read(std::shared_ptr<IOBuffer> buf, int buf_len,
                   CompletionOnceCallback callback) = 0;

  // Reads data, up to |buf_len| bytes, into |buf| without blocking. Default
  // implementation returns ERR_READ_IF_READY_NOT_IMPLEMENTED. Caller should
  // fall back to Read() if receives ERR_READ_IF_READY_NOT_IMPLEMENTED.
  // Upon synchronous completion, returns the number of bytes read, or 0 on EOF,
  // or an error code if an error happens. If read cannot be completed
  // synchronously, returns ERR_IO_PENDING and does not hold on to |buf|.
  // |callback| will be invoked with OK when data can be read, at which point,
  // caller can call ReadIfReady() again. If an error occurs asynchronously,
  // |callback| will be invoked with the error code.
  virtual int ReadIfReady(std::shared_ptr<IOBuffer> buf, int buf_len,
                          CompletionOnceCallback callback);

  // Cancels a pending ReadIfReady(). May only be called when a ReadIfReady() is
  // pending. Returns net::OK or an error code. ERR_READ_IF_READY_NOT_SUPPORTED
  // is returned if ReadIfReady() is not supported.
  virtual int CancelReadIfReady();

  // Writes data, up to |buf_len| bytes, to the socket.  Note: data may be
  // written partially.  The number of bytes written is returned, or an error
  // is returned upon failure.  ERR_SOCKET_NOT_CONNECTED should be returned if
  // the socket is not currently connected.  The return value when the
  // connection is closed is undefined, and may be OS dependent.  ERR_IO_PENDING
  // is returned if the operation could not be completed synchronously, in which
  // case the result will be passed to the callback when available.  If the
  // operation is not completed immediately, the socket acquires a reference to
  // the provided buffer until the callback is invoked or the socket is
  // closed.  Implementations of this method should not modify the contents
  // of the actual buffer that is written to the socket.  If the socket is
  // Disconnected before the write completes, the callback will not be invoked.
  virtual int Write(std::shared_ptr<IOBuffer> buf, int buf_len,
                    CompletionOnceCallback callback) = 0;

  // Set the receive buffer size (in bytes) for the socket.
  // Note: changing this value can affect the TCP window size on some platforms.
  // Returns a net error code.
  virtual int SetReceiveBufferSize(int32_t size) = 0;

  // Set the send buffer size (in bytes) for the socket.
  // Note: changing this value can affect the TCP window size on some platforms.
  // Returns a net error code.
  virtual int SetSendBufferSize(int32_t size) = 0;
};

}  // namespace base

#endif  // BASE_SOCKET_SOCKET_H_
