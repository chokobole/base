// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_UNIX_DOMAIN_CLIENT_SOCKET_POSIX_H_
#define BASE_SOCKET_UNIX_DOMAIN_CLIENT_SOCKET_POSIX_H_

#include <stdint.h>

#include <memory>
#include <string>

#include "base/completion_once_callback.h"
#include "base/export.h"
#include "base/socket/socket_descriptor.h"
#include "base/socket/stream_socket.h"

namespace base {

class SocketPosix;
struct SockaddrStorage;

// A client socket that uses unix domain socket as the transport layer.
class BASE_EXPORT UnixDomainClientSocket : public StreamSocket {
 public:
  // Builds a client socket with |socket_path|. The caller should call Connect()
  // to connect to a server socket.
  UnixDomainClientSocket(const std::string& socket_path,
                         bool use_abstract_namespace);
  // Builds a client socket with SocketPosix which is already connected.
  // UnixDomainServerSocket uses this after it accepts a connection.
  explicit UnixDomainClientSocket(std::unique_ptr<SocketPosix> socket);
  UnixDomainClientSocket(const UnixDomainClientSocket& other) = delete;
  UnixDomainClientSocket& operator=(const UnixDomainClientSocket& other) =
      delete;

  ~UnixDomainClientSocket() override;

  // Fills |address| with |socket_path| and its length. For Android or Linux
  // platform, this supports abstract namespaces.
  static bool FillAddress(const std::string& socket_path,
                          bool use_abstract_namespace,
                          SockaddrStorage* address);

  // StreamSocket implementation.
  int Connect(CompletionOnceCallback callback) override;
  void Disconnect() override;
  bool IsConnected() const override;
  bool IsConnectedAndIdle() const override;
  int GetPeerAddress(IPEndPoint* address) const override;
  int GetLocalAddress(IPEndPoint* address) const override;
  bool WasEverUsed() const override;
  int64_t GetTotalReceivedBytes() const override;

  // Socket implementation.
  int Read(std::shared_ptr<IOBuffer> buf, int buf_len,
           CompletionOnceCallback callback) override;
  int Write(std::shared_ptr<IOBuffer> buf, int buf_len,
            CompletionOnceCallback callback) override;
  int SetReceiveBufferSize(int32_t size) override;
  int SetSendBufferSize(int32_t size) override;

  // Releases ownership of underlying SocketDescriptor to caller.
  // Internal state is reset so that this object can be used again.
  // Socket must be connected in order to release it.
  SocketDescriptor ReleaseConnectedSocket();

 private:
  const std::string socket_path_;
  const bool use_abstract_namespace_;
  std::unique_ptr<SocketPosix> socket_;
};

}  // namespace base

#endif  // BASE_SOCKET_UNIX_DOMAIN_CLIENT_SOCKET_POSIX_H_
