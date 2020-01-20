// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_TCP_SERVER_SOCKET_H_
#define BASE_SOCKET_TCP_SERVER_SOCKET_H_

#include <memory>

#include "base/completion_once_callback.h"
#include "base/export.h"
#include "base/socket/ip_endpoint.h"
#include "base/socket/server_socket.h"
#include "base/socket/socket_descriptor.h"
#include "base/socket/tcp_socket.h"

namespace base {

// A server socket that uses TCP as the transport layer.
class BASE_EXPORT TCPServerSocket : public ServerSocket {
 public:
  TCPServerSocket();

  // Adopts the provided socket, which must not be a connected socket.
  explicit TCPServerSocket(std::unique_ptr<TCPSocket> socket);

  TCPServerSocket(const TCPServerSocket& other) = delete;
  TCPServerSocket& operator=(const TCPServerSocket& other) = delete;

  ~TCPServerSocket() override;

  // Takes ownership of |socket|, which has been opened, but may or may not be
  // bound or listening. The caller must determine this based on the provenance
  // of the socket and act accordingly. The socket may have connections waiting
  // to be accepted, but must not be actually connected.
  int AdoptSocket(SocketDescriptor socket);

  // net::ServerSocket implementation.
  int Listen(const IPEndPoint& address, int backlog) override;
  int GetLocalAddress(IPEndPoint* address) const override;
  int Accept(std::unique_ptr<StreamSocket>* socket,
             CompletionOnceCallback callback) override;

 private:
  // Converts |accepted_socket_| and stores the result in
  // |output_accepted_socket|.
  // |output_accepted_socket| is untouched on failure. But |accepted_socket_| is
  // set to NULL in any case.
  int ConvertAcceptedSocket(
      int result, std::unique_ptr<StreamSocket>* output_accepted_socket);
  // Completion callback for calling TCPSocket::Accept().
  void OnAcceptCompleted(std::unique_ptr<StreamSocket>* output_accepted_socket,
                         CompletionOnceCallback forward_callback, int result);

  std::unique_ptr<TCPSocket> socket_;

  std::unique_ptr<TCPSocket> accepted_socket_;
  IPEndPoint accepted_address_;
  bool pending_accept_;
};

}  // namespace base

#endif  // BASE_SOCKET_TCP_SERVER_SOCKET_H_
