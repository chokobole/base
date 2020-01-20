// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_UDP_CLIENT_SOCKET_H_
#define BASE_SOCKET_UDP_CLIENT_SOCKET_H_

#include <stdint.h>

#include "base/export.h"
#include "base/socket/datagram_client_socket.h"
#include "base/socket/udp_socket.h"

namespace base {

// A client socket that uses UDP as the transport layer.
class BASE_EXPORT UDPClientSocket : public DatagramClientSocket {
 public:
  UDPClientSocket(DatagramSocket::BindType bind_type);
  UDPClientSocket(const UDPClientSocket& other) = delete;
  UDPClientSocket& operator=(const UDPClientSocket& other) = delete;
  ~UDPClientSocket() override;

  // DatagramClientSocket implementation.
  int Connect(const IPEndPoint& address) override;
  int Read(std::shared_ptr<IOBuffer> buf, int buf_len,
           CompletionOnceCallback callback) override;
  int Write(std::shared_ptr<IOBuffer> buf, int buf_len,
            CompletionOnceCallback callback) override;

  int WriteAsync(const char* buffer, size_t buf_len,
                 CompletionOnceCallback callback) override;
  int WriteAsync(DatagramBuffers buffers,
                 CompletionOnceCallback callback) override;

  DatagramBuffers GetUnwrittenBuffers() override;

  void Close() override;
  int GetPeerAddress(IPEndPoint* address) const override;
  int GetLocalAddress(IPEndPoint* address) const override;
  // Switch to use non-blocking IO. Must be called right after construction and
  // before other calls.
  void UseNonBlockingIO() override;
  int SetReceiveBufferSize(int32_t size) override;
  int SetSendBufferSize(int32_t size) override;
  int SetDoNotFragment() override;
  void SetMsgConfirm(bool confirm) override;
  void EnableRecvOptimization() override;

  void SetWriteAsyncEnabled(bool enabled) override;
  bool WriteAsyncEnabled() override;
  void SetMaxPacketSize(size_t max_packet_size) override;
  void SetSendmmsgEnabled(bool enabled) override;
  void SetWriteBatchingActive(bool active) override;
  int SetMulticastInterface(uint32_t interface_index) override;

 private:
  UDPSocket socket_;
};

}  // namespace base

#endif  // BASE_SOCKET_UDP_CLIENT_SOCKET_H_
