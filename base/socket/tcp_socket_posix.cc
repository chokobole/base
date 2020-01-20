// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include <algorithm>

#include "base/build_config.h"
#include "base/files/file_util.h"
#include "base/io_buffer.h"
#include "base/logging.h"
#include "base/posix/eintr_wrapper.h"
#include "base/socket/address_list.h"
#include "base/socket/ip_endpoint.h"
#include "base/socket/sockaddr_storage.h"
#include "base/socket/socket_errors.h"
#include "base/socket/socket_options.h"
#include "base/socket/socket_posix.h"
#include "base/socket/tcp_socket.h"

// If we don't have a definition for TCPI_OPT_SYN_DATA, create one.
#if !defined(TCPI_OPT_SYN_DATA)
#define TCPI_OPT_SYN_DATA 32
#endif

#if defined(TCP_INFO)
#define HAVE_TCP_INFO
#endif

namespace base {

namespace {

// SetTCPKeepAlive sets SO_KEEPALIVE.
bool SetTCPKeepAlive(int fd, bool enable, int delay) {
  // Enabling TCP keepalives is the same on all platforms.
  int on = enable ? 1 : 0;
  if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on))) {
    PLOG(ERROR) << "Failed to set SO_KEEPALIVE on fd: " << fd;
    return false;
  }

  // If we disabled TCP keep alive, our work is done here.
  if (!enable) return true;

#if defined(OS_LINUX) || defined(OS_ANDROID)
  // Setting the keepalive interval varies by platform.

  // Set seconds until first TCP keep alive.
  if (setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, &delay, sizeof(delay))) {
    PLOG(ERROR) << "Failed to set TCP_KEEPIDLE on fd: " << fd;
    return false;
  }
  // Set seconds between TCP keep alives.
  if (setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &delay, sizeof(delay))) {
    PLOG(ERROR) << "Failed to set TCP_KEEPINTVL on fd: " << fd;
    return false;
  }
#elif defined(OS_MACOSX) || defined(OS_IOS)
  if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPALIVE, &delay, sizeof(delay))) {
    PLOG(ERROR) << "Failed to set TCP_KEEPALIVE on fd: " << fd;
    return false;
  }
#endif
  return true;
}

#if defined(HAVE_TCP_INFO)
// Returns a zero value if the transport RTT is unavailable.
absl::Duration GetTransportRtt(SocketDescriptor fd) {
  // It is possible for the value returned by getsockopt(TCP_INFO) to be
  // legitimately zero due to the way the RTT is calculated where fractions are
  // rounded down. This is specially true for virtualized environments with
  // paravirtualized clocks.
  //
  // If getsockopt(TCP_INFO) succeeds and the tcpi_rtt is zero, this code
  // assumes that the RTT got rounded down to zero and rounds it back up to this
  // value so that callers can assume that no packets defy the laws of physics.
  constexpr uint32_t kMinValidRttMicros = 1;

  tcp_info info;
  // Reset |tcpi_rtt| to verify if getsockopt() actually updates |tcpi_rtt|.
  info.tcpi_rtt = 0;

  socklen_t info_len = sizeof(tcp_info);
  if (getsockopt(fd, IPPROTO_TCP, TCP_INFO, &info, &info_len) != 0)
    return absl::Duration();

  // Verify that |tcpi_rtt| in tcp_info struct was updated. Note that it's
  // possible that |info_len| is shorter than |sizeof(tcp_info)| which implies
  // that only a subset of values in |info| may have been updated by
  // getsockopt().
  if (info_len < static_cast<socklen_t>(offsetof(tcp_info, tcpi_rtt) +
                                        sizeof(info.tcpi_rtt))) {
    return absl::Duration();
  }

  return absl::Microseconds(std::max(info.tcpi_rtt, kMinValidRttMicros));
}

#endif  // defined(TCP_INFO)

}  // namespace

//-----------------------------------------------------------------------------

TCPSocketPosix::TCPSocketPosix() = default;

TCPSocketPosix::~TCPSocketPosix() { Close(); }

int TCPSocketPosix::Open(AddressFamily family) {
  DCHECK(!socket_);
  socket_.reset(new SocketPosix);
  int rv = socket_->Open(ConvertAddressFamily(family));
  if (rv != OK) socket_.reset();
  return rv;
}

int TCPSocketPosix::AdoptConnectedSocket(SocketDescriptor socket,
                                         const IPEndPoint& peer_address) {
  DCHECK(!socket_);

  SockaddrStorage storage;
  if (!peer_address.ToSockAddr(storage.addr, &storage.addr_len) &&
      // For backward compatibility, allows the empty address.
      !(peer_address == IPEndPoint())) {
    return ERR_ADDRESS_INVALID;
  }

  socket_.reset(new SocketPosix);
  int rv = socket_->AdoptConnectedSocket(socket, storage);
  if (rv != OK) socket_.reset();
  return rv;
}

int TCPSocketPosix::AdoptUnconnectedSocket(SocketDescriptor socket) {
  DCHECK(!socket_);

  socket_.reset(new SocketPosix);
  int rv = socket_->AdoptUnconnectedSocket(socket);
  if (rv != OK) socket_.reset();
  return rv;
}

int TCPSocketPosix::Bind(const IPEndPoint& address) {
  DCHECK(socket_);

  SockaddrStorage storage;
  if (!address.ToSockAddr(storage.addr, &storage.addr_len))
    return ERR_ADDRESS_INVALID;

  return socket_->Bind(storage);
}

int TCPSocketPosix::Listen(int backlog) {
  DCHECK(socket_);
  return socket_->Listen(backlog);
}

int TCPSocketPosix::Accept(std::unique_ptr<TCPSocketPosix>* tcp_socket,
                           IPEndPoint* address,
                           CompletionOnceCallback callback) {
  DCHECK(tcp_socket);
  DCHECK(!callback.is_null());
  DCHECK(socket_);
  DCHECK(!accept_socket_);

  int rv = socket_->Accept(&accept_socket_,
                           [this, tcp_socket, address, callback](int rv) {
                             AcceptCompleted(tcp_socket, address, callback, rv);
                           });
  if (rv != ERR_IO_PENDING) rv = HandleAcceptCompleted(tcp_socket, address, rv);
  return rv;
}

int TCPSocketPosix::Connect(const IPEndPoint& address,
                            CompletionOnceCallback callback) {
  DCHECK(socket_);

  SockaddrStorage storage;
  if (!address.ToSockAddr(storage.addr, &storage.addr_len))
    return ERR_ADDRESS_INVALID;

  return socket_->Connect(storage, std::move(callback));
}

bool TCPSocketPosix::IsConnected() const {
  return socket_ && socket_->IsConnected();
}

bool TCPSocketPosix::IsConnectedAndIdle() const {
  return socket_ && socket_->IsConnectedAndIdle();
}

int TCPSocketPosix::Read(std::shared_ptr<IOBuffer> buf, int buf_len,
                         CompletionOnceCallback callback) {
  DCHECK(socket_);
  DCHECK(!callback.is_null());

  return socket_->Read(buf, buf_len, std::move(callback));
}

int TCPSocketPosix::ReadIfReady(std::shared_ptr<IOBuffer> buf, int buf_len,
                                CompletionOnceCallback callback) {
  DCHECK(socket_);
  DCHECK(!callback.is_null());

  return socket_->ReadIfReady(buf, buf_len, std::move(callback));
}

int TCPSocketPosix::CancelReadIfReady() {
  DCHECK(socket_);

  return socket_->CancelReadIfReady();
}

int TCPSocketPosix::Write(std::shared_ptr<IOBuffer> buf, int buf_len,
                          CompletionOnceCallback callback) {
  DCHECK(socket_);
  DCHECK(!callback.is_null());

  return socket_->Write(buf, buf_len, std::move(callback));
}

int TCPSocketPosix::GetLocalAddress(IPEndPoint* address) const {
  DCHECK(address);

  if (!socket_) return ERR_SOCKET_NOT_CONNECTED;

  SockaddrStorage storage;
  int rv = socket_->GetLocalAddress(&storage);
  if (rv != OK) return rv;

  if (!address->FromSockAddr(storage.addr, storage.addr_len))
    return ERR_ADDRESS_INVALID;

  return OK;
}

int TCPSocketPosix::GetPeerAddress(IPEndPoint* address) const {
  DCHECK(address);

  if (!IsConnected()) return ERR_SOCKET_NOT_CONNECTED;

  SockaddrStorage storage;
  int rv = socket_->GetPeerAddress(&storage);
  if (rv != OK) return rv;

  if (!address->FromSockAddr(storage.addr, storage.addr_len))
    return ERR_ADDRESS_INVALID;

  return OK;
}

int TCPSocketPosix::SetDefaultOptionsForServer() {
  DCHECK(socket_);
  return AllowAddressReuse();
}

void TCPSocketPosix::SetDefaultOptionsForClient() {
  DCHECK(socket_);

  // This mirrors the behaviour on Windows. See the comment in
  // tcp_socket_win.cc after searching for "NODELAY".
  // If SetTCPNoDelay fails, we don't care.
  SetTCPNoDelay(socket_->socket_fd(), true);

  // TCP keep alive wakes up the radio, which is expensive on mobile. Do not
  // enable it there. It's useful to prevent TCP middleboxes from timing out
  // connection mappings. Packets for timed out connection mappings at
  // middleboxes will either lead to:
  // a) Middleboxes sending TCP RSTs. It's up to higher layers to check for this
  // and retry. The HTTP network transaction code does this.
  // b) Middleboxes just drop the unrecognized TCP packet. This leads to the TCP
  // stack retransmitting packets per TCP stack retransmission timeouts, which
  // are very high (on the order of seconds). Given the number of
  // retransmissions required before killing the connection, this can lead to
  // tens of seconds or even minutes of delay, depending on OS.
#if !defined(OS_ANDROID) && !defined(OS_IOS)
  const int kTCPKeepAliveSeconds = 45;

  SetTCPKeepAlive(socket_->socket_fd(), true, kTCPKeepAliveSeconds);
#endif
}

int TCPSocketPosix::AllowAddressReuse() {
  DCHECK(socket_);

  return SetReuseAddr(socket_->socket_fd(), true);
}

int TCPSocketPosix::SetReceiveBufferSize(int32_t size) {
  DCHECK(socket_);

  return SetSocketReceiveBufferSize(socket_->socket_fd(), size);
}

int TCPSocketPosix::SetSendBufferSize(int32_t size) {
  DCHECK(socket_);

  return SetSocketSendBufferSize(socket_->socket_fd(), size);
}

bool TCPSocketPosix::SetKeepAlive(bool enable, int delay) {
  DCHECK(socket_);

  return SetTCPKeepAlive(socket_->socket_fd(), enable, delay);
}

bool TCPSocketPosix::SetNoDelay(bool no_delay) {
  DCHECK(socket_);

  return SetTCPNoDelay(socket_->socket_fd(), no_delay) == OK;
}

void TCPSocketPosix::Close() { socket_.reset(); }

bool TCPSocketPosix::IsValid() const {
  return socket_ != NULL && socket_->socket_fd() != kInvalidSocket;
}

void TCPSocketPosix::AcceptCompleted(
    std::unique_ptr<TCPSocketPosix>* tcp_socket, IPEndPoint* address,
    CompletionOnceCallback callback, int rv) {
  DCHECK_NE(ERR_IO_PENDING, rv);
  std::move(callback).Run(HandleAcceptCompleted(tcp_socket, address, rv));
}

int TCPSocketPosix::HandleAcceptCompleted(
    std::unique_ptr<TCPSocketPosix>* tcp_socket, IPEndPoint* address, int rv) {
  if (rv == OK) rv = BuildTcpSocketPosix(tcp_socket, address);
  return rv;
}

int TCPSocketPosix::BuildTcpSocketPosix(
    std::unique_ptr<TCPSocketPosix>* tcp_socket, IPEndPoint* address) {
  DCHECK(accept_socket_);

  SockaddrStorage storage;
  if (accept_socket_->GetPeerAddress(&storage) != OK ||
      !address->FromSockAddr(storage.addr, storage.addr_len)) {
    accept_socket_.reset();
    return ERR_ADDRESS_INVALID;
  }

  tcp_socket->reset(new TCPSocketPosix());
  (*tcp_socket)->socket_ = std::move(accept_socket_);
  return OK;
}

bool TCPSocketPosix::GetEstimatedRoundTripTime(absl::Duration* out_rtt) const {
  DCHECK(out_rtt);
  if (!socket_) return false;

#if defined(HAVE_TCP_INFO)
  absl::Duration rtt = GetTransportRtt(socket_->socket_fd());
  if (rtt == absl::Duration()) return false;
  *out_rtt = rtt;
  return true;
#endif  // defined(TCP_INFO)
  return false;
}

}  // namespace base
