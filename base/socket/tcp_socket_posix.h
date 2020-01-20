// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_TCP_SOCKET_POSIX_H_
#define BASE_SOCKET_TCP_SOCKET_POSIX_H_

#include <stdint.h>

#include <memory>

#include "absl/time/time.h"
#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/completion_once_callback.h"
#include "base/export.h"
#include "base/socket/address_family.h"
#include "base/socket/socket_descriptor.h"

namespace base {

class AddressList;
class IOBuffer;
class IPEndPoint;
class SocketPosix;

class BASE_EXPORT TCPSocketPosix {
 public:
  TCPSocketPosix();
  TCPSocketPosix(const TCPSocketPosix& other) = delete;
  TCPSocketPosix& operator=(const TCPSocketPosix& other) = delete;
  virtual ~TCPSocketPosix();

  // Opens the socket.
  // Returns a net error code.
  int Open(AddressFamily family);

  // Takes ownership of |socket|, which is known to already be connected to the
  // given peer address. However, peer address may be the empty address, for
  // compatibility. The given peer address will be returned by GetPeerAddress.
  int AdoptConnectedSocket(SocketDescriptor socket,
                           const IPEndPoint& peer_address);
  // Takes ownership of |socket|, which may or may not be open, bound, or
  // listening. The caller must determine the state of the socket based on its
  // provenance and act accordingly. The socket may have connections waiting
  // to be accepted, but must not be actually connected.
  int AdoptUnconnectedSocket(SocketDescriptor socket);

  // Binds this socket to |address|. This is generally only used on a server.
  // Should be called after Open(). Returns a net error code.
  int Bind(const IPEndPoint& address);

  // Put this socket on listen state with the given |backlog|.
  // Returns a net error code.
  int Listen(int backlog);

  // Accepts incoming connection.
  // Returns a net error code.
  int Accept(std::unique_ptr<TCPSocketPosix>* socket, IPEndPoint* address,
             CompletionOnceCallback callback);

  // Connects this socket to the given |address|.
  // Should be called after Open().
  // Returns a net error code.
  int Connect(const IPEndPoint& address, CompletionOnceCallback callback);
  bool IsConnected() const;
  bool IsConnectedAndIdle() const;

  // IO:
  // Multiple outstanding requests are not supported.
  // Full duplex mode (reading and writing at the same time) is supported.

  // Reads from the socket.
  // Returns a net error code.
  int Read(std::shared_ptr<IOBuffer> buf, int buf_len,
           CompletionOnceCallback callback);
  int ReadIfReady(std::shared_ptr<IOBuffer> buf, int buf_len,
                  CompletionOnceCallback callback);
  int CancelReadIfReady();

  // Writes to the socket.
  // Returns a net error code.
  int Write(std::shared_ptr<IOBuffer> buf, int buf_len,
            CompletionOnceCallback callback);

  // Copies the local tcp address into |address| and returns a net error code.
  int GetLocalAddress(IPEndPoint* address) const;

  // Copies the remote tcp code into |address| and returns a net error code.
  int GetPeerAddress(IPEndPoint* address) const;

  // Sets various socket options.
  // The commonly used options for server listening sockets:
  // - AllowAddressReuse().
  int SetDefaultOptionsForServer();
  // The commonly used options for client sockets and accepted sockets:
  // - SetNoDelay(true);
  // - SetKeepAlive(true, 45).
  void SetDefaultOptionsForClient();
  int AllowAddressReuse();
  int SetReceiveBufferSize(int32_t size);
  int SetSendBufferSize(int32_t size);
  bool SetKeepAlive(bool enable, int delay);
  bool SetNoDelay(bool no_delay);

  // Gets the estimated RTT. Returns false if the RTT is
  // unavailable. May also return false when estimated RTT is 0.
  bool GetEstimatedRoundTripTime(absl::Duration* out_rtt) const
      WARN_UNUSED_RESULT;

  // Closes the socket.
  void Close();

  bool IsValid() const;

 private:
  void AcceptCompleted(std::unique_ptr<TCPSocketPosix>* tcp_socket,
                       IPEndPoint* address, CompletionOnceCallback callback,
                       int rv);
  int HandleAcceptCompleted(std::unique_ptr<TCPSocketPosix>* tcp_socket,
                            IPEndPoint* address, int rv);
  int BuildTcpSocketPosix(std::unique_ptr<TCPSocketPosix>* tcp_socket,
                          IPEndPoint* address);

  std::unique_ptr<SocketPosix> socket_;
  std::unique_ptr<SocketPosix> accept_socket_;
};

}  // namespace base

#endif  // BASE_SOCKET_TCP_SOCKET_POSIX_H_
