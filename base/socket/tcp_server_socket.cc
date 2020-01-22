// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/socket/tcp_server_socket.h"

#include <utility>

#include "absl/functional/bind_front.h"
#include "base/logging.h"
#include "base/socket/socket_descriptor.h"
#include "base/socket/socket_errors.h"
#include "base/socket/tcp_client_socket.h"

namespace base {

TCPServerSocket::TCPServerSocket()
    : TCPServerSocket(std::make_unique<TCPSocket>()) {}

TCPServerSocket::TCPServerSocket(std::unique_ptr<TCPSocket> socket)
    : socket_(std::move(socket)) {}

int TCPServerSocket::AdoptSocket(SocketDescriptor socket) {
  return socket_->AdoptUnconnectedSocket(socket);
}

TCPServerSocket::~TCPServerSocket() = default;

int TCPServerSocket::Listen(const IPEndPoint& address, int backlog) {
  int result = socket_->Open(address.GetFamily());
  if (result != OK) return result;

  result = socket_->SetDefaultOptionsForServer();
  if (result != OK) {
    socket_->Close();
    return result;
  }

  result = socket_->Bind(address);
  if (result != OK) {
    socket_->Close();
    return result;
  }

  result = socket_->Listen(backlog);
  if (result != OK) {
    socket_->Close();
    return result;
  }

  return OK;
}

int TCPServerSocket::GetLocalAddress(IPEndPoint* address) const {
  return socket_->GetLocalAddress(address);
}

int TCPServerSocket::Accept(std::unique_ptr<StreamSocket>* socket,
                            CompletionOnceCallback callback) {
  DCHECK(socket);
  DCHECK(!callback.is_null());
  DCHECK(accept_callback_.is_null());

  // |socket_| is owned by this class, and the callback won't be run after
  // |socket_| is destroyed.
  int result = socket_->Accept(
      &accepted_socket_, &accepted_address_,
      absl::bind_front(&TCPServerSocket::OnAcceptCompleted, this, socket));
  if (result != ERR_IO_PENDING) {
    // |accept_callback| won't be called so we need to run
    // ConvertAcceptedSocket() ourselves in order to do the conversion from
    // |accepted_socket_| to |socket|.
    result = ConvertAcceptedSocket(result, socket);
  } else {
    accept_callback_ = std::move(callback);
  }

  return result;
}

int TCPServerSocket::ConvertAcceptedSocket(
    int result, std::unique_ptr<StreamSocket>* output_accepted_socket) {
  // Make sure the TCPSocket object is destroyed in any case.
  std::unique_ptr<TCPSocket> temp_accepted_socket(std::move(accepted_socket_));
  if (result != OK) return result;

  output_accepted_socket->reset(
      new TCPClientSocket(std::move(temp_accepted_socket), accepted_address_));

  return OK;
}

void TCPServerSocket::OnAcceptCompleted(
    std::unique_ptr<StreamSocket>* output_accepted_socket, int result) {
  DCHECK(!accept_callback_.is_null());
  result = ConvertAcceptedSocket(result, output_accepted_socket);
  std::move(accept_callback_).Run(result);
}

}  // namespace base
