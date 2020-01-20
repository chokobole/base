// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/socket/udp_client_socket.h"

#include "base/build_config.h"
#include "base/socket/socket_errors.h"

namespace base {

UDPClientSocket::UDPClientSocket(DatagramSocket::BindType bind_type)
    : socket_(bind_type) {}

UDPClientSocket::~UDPClientSocket() = default;

int UDPClientSocket::Connect(const IPEndPoint& address) {
  int rv = socket_.Open(address.GetFamily());
  if (rv != OK) return rv;
  return socket_.Connect(address);
}

int UDPClientSocket::Read(std::shared_ptr<IOBuffer> buf, int buf_len,
                          CompletionOnceCallback callback) {
  return socket_.Read(buf, buf_len, std::move(callback));
}

int UDPClientSocket::Write(std::shared_ptr<IOBuffer> buf, int buf_len,
                           CompletionOnceCallback callback) {
  return socket_.Write(buf, buf_len, std::move(callback));
}

int UDPClientSocket::WriteAsync(const char* buffer, size_t buf_len,
                                CompletionOnceCallback callback) {
  DCHECK(WriteAsyncEnabled());
  return socket_.WriteAsync(buffer, buf_len, std::move(callback));
}

int UDPClientSocket::WriteAsync(DatagramBuffers buffers,
                                CompletionOnceCallback callback) {
  DCHECK(WriteAsyncEnabled());
  return socket_.WriteAsync(std::move(buffers), std::move(callback));
}

DatagramBuffers UDPClientSocket::GetUnwrittenBuffers() {
  return socket_.GetUnwrittenBuffers();
}

void UDPClientSocket::Close() { socket_.Close(); }

int UDPClientSocket::GetPeerAddress(IPEndPoint* address) const {
  return socket_.GetPeerAddress(address);
}

int UDPClientSocket::GetLocalAddress(IPEndPoint* address) const {
  return socket_.GetLocalAddress(address);
}

int UDPClientSocket::SetReceiveBufferSize(int32_t size) {
  return socket_.SetReceiveBufferSize(size);
}

int UDPClientSocket::SetSendBufferSize(int32_t size) {
  return socket_.SetSendBufferSize(size);
}

int UDPClientSocket::SetDoNotFragment() { return socket_.SetDoNotFragment(); }

void UDPClientSocket::SetMsgConfirm(bool confirm) {
  socket_.SetMsgConfirm(confirm);
}

void UDPClientSocket::UseNonBlockingIO() {
#if defined(OS_WIN)
  socket_.UseNonBlockingIO();
#endif
}

void UDPClientSocket::SetWriteAsyncEnabled(bool enabled) {
  socket_.SetWriteAsyncEnabled(enabled);
}

void UDPClientSocket::SetMaxPacketSize(size_t max_packet_size) {
  socket_.SetMaxPacketSize(max_packet_size);
}

bool UDPClientSocket::WriteAsyncEnabled() {
  return socket_.WriteAsyncEnabled();
}

void UDPClientSocket::SetSendmmsgEnabled(bool enabled) {
  socket_.SetSendmmsgEnabled(enabled);
}

void UDPClientSocket::SetWriteBatchingActive(bool active) {
  socket_.SetWriteBatchingActive(active);
}

int UDPClientSocket::SetMulticastInterface(uint32_t interface_index) {
  return socket_.SetMulticastInterface(interface_index);
}

void UDPClientSocket::EnableRecvOptimization() {
#if defined(OS_POSIX)
  socket_.enable_experimental_recv_optimization();
#endif
}

}  // namespace base
