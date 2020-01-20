// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_STREAM_SOCKET_H_
#define BASE_SOCKET_STREAM_SOCKET_H_

#include <stddef.h>
#include <stdint.h>

#include "base/export.h"
#include "base/socket/socket.h"

namespace base {

class IPEndPoint;

class BASE_EXPORT StreamSocket : public Socket {
 public:
  using BeforeConnectCallback = RepeatingCallback<int()>;

  // This is used in DumpMemoryStats() to track the estimate of memory usage of
  // a socket.
  struct BASE_EXPORT SocketMemoryStats {
   public:
    SocketMemoryStats();
    SocketMemoryStats(const SocketMemoryStats& other) = delete;
    SocketMemoryStats& operator=(const SocketMemoryStats& other) = delete;
    ~SocketMemoryStats();
    // Estimated total memory usage of this socket in bytes.
    size_t total_size;
    // Size of all buffers used by this socket in bytes.
    size_t buffer_size;
    // Number of certs used by this socket.
    size_t cert_count;
    // Total size of certs used by this socket in bytes.
    size_t cert_size;
  };

  ~StreamSocket() override {}

  // Sets a callback to be invoked before establishing a connection. This allows
  // setting options, like receive and send buffer size, when they will take
  // effect. The callback should return net::OK on success, and an error on
  // failure. It must not return net::ERR_IO_PENDING.
  //
  // If multiple connection attempts are made, the callback will be invoked for
  // each one.
  virtual void SetBeforeConnectCallback(
      const BeforeConnectCallback& before_connect_callback);

  // Called to establish a connection.  Returns OK if the connection could be
  // established synchronously.  Otherwise, ERR_IO_PENDING is returned and the
  // given callback will run asynchronously when the connection is established
  // or when an error occurs.  The result is some other error code if the
  // connection could not be established.
  //
  // The socket's Read and Write methods may not be called until Connect
  // succeeds.
  //
  // It is valid to call Connect on an already connected socket, in which case
  // OK is simply returned.
  //
  // Connect may also be called again after a call to the Disconnect method.
  //
  virtual int Connect(CompletionOnceCallback callback) = 0;

  // Called to confirm the TLS handshake, if any, indicating that replay
  // protection is ready. Returns OK if the handshake could complete
  // synchronously or had already been confirmed. Otherwise, ERR_IO_PENDING is
  // returned and the given callback will run asynchronously when the connection
  // is established or when an error occurs.  The result is some other error
  // code if the connection could not be completed.
  //
  // This operation is only needed if TLS early data is enabled, in which case
  // Connect returns early and Write initially sends early data, which does not
  // have TLS's usual security properties. The caller must call this function
  // and wait for handshake confirmation before sending data that is not
  // replay-safe.
  //
  // ConfirmHandshake may run concurrently with Read or Write, but, as with Read
  // and Write, at most one pending ConfirmHandshake operation may be in
  // progress at a time.
  virtual int ConfirmHandshake(CompletionOnceCallback callback);

  // Called to disconnect a socket.  Does nothing if the socket is already
  // disconnected.  After calling Disconnect it is possible to call Connect
  // again to establish a new connection.
  //
  // If IO (Connect, Read, or Write) is pending when the socket is
  // disconnected, the pending IO is cancelled, and the completion callback
  // will not be called.
  virtual void Disconnect() = 0;

  // Called to test if the connection is still alive.  Returns false if a
  // connection wasn't established or the connection is dead.  True is returned
  // if the connection was terminated, but there is unread data in the incoming
  // buffer.
  virtual bool IsConnected() const = 0;

  // Called to test if the connection is still alive and idle.  Returns false
  // if a connection wasn't established, the connection is dead, or there is
  // unread data in the incoming buffer.
  virtual bool IsConnectedAndIdle() const = 0;

  // Copies the peer address to |address| and returns a network error code.
  // ERR_SOCKET_NOT_CONNECTED will be returned if the socket is not connected.
  virtual int GetPeerAddress(IPEndPoint* address) const = 0;

  // Copies the local address to |address| and returns a network error code.
  // ERR_SOCKET_NOT_CONNECTED will be returned if the socket is not bound.
  virtual int GetLocalAddress(IPEndPoint* address) const = 0;

  // Returns true if the socket ever had any reads or writes.  StreamSockets
  // layered on top of transport sockets should return if their own Read() or
  // Write() methods had been called, not the underlying transport's.
  virtual bool WasEverUsed() const = 0;

  // Returns the total number of number bytes read by the socket. This only
  // counts the payload bytes. Transport headers are not counted. Returns
  // 0 if the socket does not implement the function. The count is reset when
  // Disconnect() is called.
  virtual int64_t GetTotalReceivedBytes() const = 0;

  // Dumps memory allocation stats into |stats|. |stats| can be assumed as being
  // default initialized upon entry. Implementations should override fields in
  // |stats|. Default implementation does nothing.
  virtual void DumpMemoryStats(SocketMemoryStats* stats) const {}
};

}  // namespace base

#endif  // BASE_SOCKET_STREAM_SOCKET_H_
