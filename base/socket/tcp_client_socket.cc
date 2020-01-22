// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/socket/tcp_client_socket.h"

#include <utility>

#include "absl/functional/bind_front.h"
#include "absl/memory/memory.h"
#include "base/io_buffer.h"
#include "base/logging.h"
#include "base/socket/ip_endpoint.h"
#include "base/socket/socket_errors.h"

namespace base {

TCPClientSocket::TCPClientSocket(const AddressList& addresses)
    : TCPClientSocket(std::make_unique<TCPSocket>(), addresses,
                      -1 /* current_address_index */,
                      nullptr /* bind_address */) {}

TCPClientSocket::TCPClientSocket(std::unique_ptr<TCPSocket> connected_socket,
                                 const IPEndPoint& peer_address)
    : TCPClientSocket(std::move(connected_socket), AddressList(peer_address),
                      0 /* current_address_index */,
                      nullptr /* bind_address */) {}

TCPClientSocket::~TCPClientSocket() { Disconnect(); }

std::unique_ptr<TCPClientSocket> TCPClientSocket::CreateFromBoundSocket(
    std::unique_ptr<TCPSocket> bound_socket, const AddressList& addresses,
    const IPEndPoint& bound_address) {
  return absl::WrapUnique(new TCPClientSocket(
      std::move(bound_socket), addresses, -1 /* current_address_index */,
      std::make_unique<IPEndPoint>(bound_address)));
}

int TCPClientSocket::Bind(const IPEndPoint& address) {
  if (current_address_index_ >= 0 || bind_address_) {
    // Cannot bind the socket if we are already connected or connecting.
    NOTREACHED();
    return ERR_UNEXPECTED;
  }

  int result = OK;
  if (!socket_->IsValid()) {
    result = OpenSocket(address.GetFamily());
    if (result != OK) return result;
  }

  result = socket_->Bind(address);
  if (result != OK) return result;

  bind_address_.reset(new IPEndPoint(address));
  return OK;
}

bool TCPClientSocket::SetKeepAlive(bool enable, int delay) {
  return socket_->SetKeepAlive(enable, delay);
}

bool TCPClientSocket::SetNoDelay(bool no_delay) {
  return socket_->SetNoDelay(no_delay);
}

void TCPClientSocket::SetBeforeConnectCallback(
    const BeforeConnectCallback& before_connect_callback) {
  DCHECK_EQ(CONNECT_STATE_NONE, next_connect_state_);
  before_connect_callback_ = before_connect_callback;
}

int TCPClientSocket::Connect(CompletionOnceCallback callback) {
  DCHECK(!callback.is_null());

  // If connecting or already connected, then just return OK.
  if (socket_->IsValid() && current_address_index_ >= 0) return OK;

  DCHECK(!read_callback_);
  DCHECK(!write_callback_);

  if (was_disconnected_on_suspend_) {
    Disconnect();
    was_disconnected_on_suspend_ = false;
  }

  // We will try to connect to each address in addresses_. Start with the
  // first one in the list.
  next_connect_state_ = CONNECT_STATE_CONNECT;
  current_address_index_ = 0;

  int rv = DoConnectLoop(OK);
  if (rv == ERR_IO_PENDING) {
    connect_callback_ = std::move(callback);
  }

  return rv;
}

TCPClientSocket::TCPClientSocket(std::unique_ptr<TCPSocket> socket,
                                 const AddressList& addresses,
                                 int current_address_index,
                                 std::unique_ptr<IPEndPoint> bind_address)
    : socket_(std::move(socket)),
      bind_address_(std::move(bind_address)),
      addresses_(addresses),
      current_address_index_(-1),
      next_connect_state_(CONNECT_STATE_NONE),
      previously_disconnected_(false),
      total_received_bytes_(0),
      was_ever_used_(false),
      was_disconnected_on_suspend_(false) {
  DCHECK(socket_);
  if (socket_->IsValid()) socket_->SetDefaultOptionsForClient();
}

int TCPClientSocket::ReadCommon(std::shared_ptr<IOBuffer> buf, int buf_len,
                                CompletionOnceCallback callback,
                                bool read_if_ready) {
  DCHECK(!callback.is_null());
  DCHECK(read_callback_.is_null());

  if (was_disconnected_on_suspend_) return ERR_NETWORK_IO_SUSPENDED;

  // |socket_| is owned by |this| and the callback won't be run once |socket_|
  // is gone/closed.
  CompletionOnceCallback complete_read_callback(
      absl::bind_front(&TCPClientSocket::DidCompleteRead, this));
  int result =
      read_if_ready
          ? socket_->ReadIfReady(buf, buf_len,
                                 std::move(complete_read_callback))
          : socket_->Read(buf, buf_len, std::move(complete_read_callback));
  if (result == ERR_IO_PENDING) {
    read_callback_ = std::move(callback);
  } else if (result > 0) {
    was_ever_used_ = true;
    total_received_bytes_ += result;
  }

  return result;
}

int TCPClientSocket::DoConnectLoop(int result) {
  DCHECK_NE(next_connect_state_, CONNECT_STATE_NONE);

  int rv = result;
  do {
    ConnectState state = next_connect_state_;
    next_connect_state_ = CONNECT_STATE_NONE;
    switch (state) {
      case CONNECT_STATE_CONNECT:
        DCHECK_EQ(OK, rv);
        rv = DoConnect();
        break;
      case CONNECT_STATE_CONNECT_COMPLETE:
        rv = DoConnectComplete(rv);
        break;
      default:
        NOTREACHED() << "bad state " << state;
        rv = ERR_UNEXPECTED;
        break;
    }
  } while (rv != ERR_IO_PENDING && next_connect_state_ != CONNECT_STATE_NONE);

  return rv;
}

int TCPClientSocket::DoConnect() {
  DCHECK_GE(current_address_index_, 0);
  DCHECK_LT(current_address_index_, static_cast<int>(addresses_.size()));

  const IPEndPoint& endpoint = addresses_[current_address_index_];

  if (previously_disconnected_) {
    was_ever_used_ = false;
    previously_disconnected_ = false;
  }

  next_connect_state_ = CONNECT_STATE_CONNECT_COMPLETE;

  if (socket_->IsValid()) {
    DCHECK(bind_address_);
  } else {
    int result = OpenSocket(endpoint.GetFamily());
    if (result != OK) return result;

    if (bind_address_) {
      result = socket_->Bind(*bind_address_);
      if (result != OK) {
        socket_->Close();
        return result;
      }
    }
  }

  if (before_connect_callback_) {
    int result = before_connect_callback_.Run();
    DCHECK_NE(ERR_IO_PENDING, result);
    if (result != OK) return result;
  }

  return ConnectInternal(endpoint);
}

int TCPClientSocket::DoConnectComplete(int result) {
  if (result == OK) return OK;  // Done!

  // Don't try the next address if entering suspend mode.
  if (result == ERR_NETWORK_IO_SUSPENDED) return result;

  // Close whatever partially connected socket we currently have.
  DoDisconnect();

  // Try to fall back to the next address in the list.
  if (current_address_index_ + 1 < static_cast<int>(addresses_.size())) {
    next_connect_state_ = CONNECT_STATE_CONNECT;
    ++current_address_index_;
    return OK;
  }

  // Otherwise there is nothing to fall back to, so give up.
  return result;
}

int TCPClientSocket::ConnectInternal(const IPEndPoint& endpoint) {
  // |socket_| is owned by this class and the callback won't be run once
  // |socket_| is gone.
  return socket_->Connect(
      endpoint, absl::bind_front(&TCPClientSocket::DidCompleteConnect, this));
}

void TCPClientSocket::Disconnect() {
  DoDisconnect();
  current_address_index_ = -1;
  bind_address_.reset();

  // Cancel any pending callbacks. Not done in DoDisconnect() because that's
  // called on connection failure, when the connect callback will need to be
  // invoked.
  was_disconnected_on_suspend_ = false;
  connect_callback_.Reset();
  read_callback_.Reset();
  write_callback_.Reset();
}

void TCPClientSocket::DoDisconnect() {
  total_received_bytes_ = 0;

  // If connecting or already connected, record that the socket has been
  // disconnected.
  previously_disconnected_ = socket_->IsValid() && current_address_index_ >= 0;
  socket_->Close();
}

bool TCPClientSocket::IsConnected() const { return socket_->IsConnected(); }

bool TCPClientSocket::IsConnectedAndIdle() const {
  return socket_->IsConnectedAndIdle();
}

int TCPClientSocket::GetPeerAddress(IPEndPoint* address) const {
  return socket_->GetPeerAddress(address);
}

int TCPClientSocket::GetLocalAddress(IPEndPoint* address) const {
  DCHECK(address);

  if (!socket_->IsValid()) {
    if (bind_address_) {
      *address = *bind_address_;
      return OK;
    }
    return ERR_SOCKET_NOT_CONNECTED;
  }

  return socket_->GetLocalAddress(address);
}

bool TCPClientSocket::WasEverUsed() const { return was_ever_used_; }

int TCPClientSocket::Read(std::shared_ptr<IOBuffer> buf, int buf_len,
                          CompletionOnceCallback callback) {
  return ReadCommon(buf, buf_len, std::move(callback), /*read_if_ready=*/false);
}

int TCPClientSocket::ReadIfReady(std::shared_ptr<IOBuffer> buf, int buf_len,
                                 CompletionOnceCallback callback) {
  return ReadCommon(buf, buf_len, std::move(callback), /*read_if_ready=*/true);
}

int TCPClientSocket::CancelReadIfReady() {
  DCHECK(read_callback_);
  read_callback_.Reset();
  return socket_->CancelReadIfReady();
}

int TCPClientSocket::Write(std::shared_ptr<IOBuffer> buf, int buf_len,
                           CompletionOnceCallback callback) {
  DCHECK(!callback.is_null());
  DCHECK(write_callback_.is_null());

  if (was_disconnected_on_suspend_) return ERR_NETWORK_IO_SUSPENDED;

  // |socket_| is owned by this class and the callback won't be run once
  // |socket_| is gone.
  int result = socket_->Write(
      buf, buf_len, absl::bind_front(&TCPClientSocket::DidCompleteWrite, this));
  if (result == ERR_IO_PENDING) {
    write_callback_ = std::move(callback);
  } else if (result > 0) {
    was_ever_used_ = true;
  }

  return result;
}

int TCPClientSocket::SetReceiveBufferSize(int32_t size) {
  return socket_->SetReceiveBufferSize(size);
}

int TCPClientSocket::SetSendBufferSize(int32_t size) {
  return socket_->SetSendBufferSize(size);
}

int64_t TCPClientSocket::GetTotalReceivedBytes() const {
  return total_received_bytes_;
}

void TCPClientSocket::DidCompleteConnect(int result) {
  DCHECK_EQ(next_connect_state_, CONNECT_STATE_CONNECT_COMPLETE);
  DCHECK_NE(result, ERR_IO_PENDING);
  DCHECK(!connect_callback_.is_null());

  result = DoConnectLoop(result);
  if (result != ERR_IO_PENDING) {
    std::move(connect_callback_).Run(result);
  }
}

void TCPClientSocket::DidCompleteRead(int result) {
  DCHECK(!read_callback_.is_null());

  if (result > 0) total_received_bytes_ += result;
  DidCompleteReadWrite(std::move(read_callback_), result);
}

void TCPClientSocket::DidCompleteWrite(int result) {
  DCHECK(!write_callback_.is_null());

  DidCompleteReadWrite(std::move(write_callback_), result);
}

void TCPClientSocket::DidCompleteReadWrite(CompletionOnceCallback callback,
                                           int result) {
  if (result > 0) was_ever_used_ = true;
  std::move(callback).Run(result);
}

int TCPClientSocket::OpenSocket(AddressFamily family) {
  DCHECK(!socket_->IsValid());

  int result = socket_->Open(family);
  if (result != OK) return result;

  socket_->SetDefaultOptionsForClient();

  return OK;
}

}  // namespace base
