// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/socket/unix_domain_server_socket_posix.h"

#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <utility>

#include "base/build_config.h"
#include "base/logging.h"
#include "base/socket/sockaddr_storage.h"
#include "base/socket/socket_errors.h"
#include "base/socket/socket_posix.h"
#include "base/socket/unix_domain_client_socket_posix.h"

namespace base {

namespace {

// Intended for use as SetterCallbacks in Accept() helper methods.
void SetStreamSocket(std::unique_ptr<StreamSocket>* socket,
                     std::unique_ptr<SocketPosix> accepted_socket) {
  socket->reset(new UnixDomainClientSocket(std::move(accepted_socket)));
}

void SetSocketDescriptor(SocketDescriptor* socket,
                         std::unique_ptr<SocketPosix> accepted_socket) {
  *socket = accepted_socket->ReleaseConnectedSocket();
}

}  // anonymous namespace

UnixDomainServerSocket::UnixDomainServerSocket(
    const AuthCallback& auth_callback, bool use_abstract_namespace)
    : auth_callback_(auth_callback),
      use_abstract_namespace_(use_abstract_namespace) {
  DCHECK(!auth_callback_.is_null());
}

UnixDomainServerSocket::~UnixDomainServerSocket() = default;

// static
bool UnixDomainServerSocket::GetPeerCredentials(SocketDescriptor socket,
                                                Credentials* credentials) {
#if defined(OS_LINUX) || defined(OS_ANDROID)
  struct ucred user_cred;
  socklen_t len = sizeof(user_cred);
  if (getsockopt(socket, SOL_SOCKET, SO_PEERCRED, &user_cred, &len) < 0)
    return false;
  credentials->process_id = user_cred.pid;
  credentials->user_id = user_cred.uid;
  credentials->group_id = user_cred.gid;
  return true;
#else
  return getpeereid(socket, &credentials->user_id, &credentials->group_id) == 0;
#endif
}

int UnixDomainServerSocket::Listen(const IPEndPoint& address, int backlog) {
  NOTIMPLEMENTED();
  return ERR_NOT_IMPLEMENTED;
}

int UnixDomainServerSocket::ListenWithAddressAndPort(
    const std::string& address_string, uint16_t port, int backlog) {
  NOTIMPLEMENTED();
  return ERR_NOT_IMPLEMENTED;
}

int UnixDomainServerSocket::BindAndListen(const std::string& socket_path,
                                          int backlog) {
  DCHECK(!listen_socket_);

  SockaddrStorage address;
  if (!UnixDomainClientSocket::FillAddress(socket_path, use_abstract_namespace_,
                                           &address)) {
    return ERR_ADDRESS_INVALID;
  }

  std::unique_ptr<SocketPosix> socket(new SocketPosix);
  int rv = socket->Open(AF_UNIX);
  DCHECK_NE(ERR_IO_PENDING, rv);
  if (rv != OK) return rv;

  rv = socket->Bind(address);
  DCHECK_NE(ERR_IO_PENDING, rv);
  if (rv != OK) {
    PLOG(ERROR) << "Could not bind unix domain socket to " << socket_path
                << (use_abstract_namespace_ ? " (with abstract namespace)"
                                            : "");
    return rv;
  }

  rv = socket->Listen(backlog);
  DCHECK_NE(ERR_IO_PENDING, rv);
  if (rv != OK) return rv;

  listen_socket_.swap(socket);
  return rv;
}

int UnixDomainServerSocket::GetLocalAddress(IPEndPoint* address) const {
  DCHECK(address);

  // Unix domain sockets have no valid associated addr/port;
  // return address invalid.
  return ERR_ADDRESS_INVALID;
}

int UnixDomainServerSocket::Accept(std::unique_ptr<StreamSocket>* socket,
                                   CompletionOnceCallback callback) {
  DCHECK(socket);
  DCHECK(callback);
  DCHECK(!callback_);

  SetterCallback setter_callback(
      [socket](std::unique_ptr<SocketPosix> accepted_socket) {
        SetStreamSocket(socket, std::move(accepted_socket));
      });
  int rv = DoAccept(setter_callback);
  if (rv == ERR_IO_PENDING) callback_ = std::move(callback);
  return rv;
}

int UnixDomainServerSocket::AcceptSocketDescriptor(
    SocketDescriptor* socket, CompletionOnceCallback callback) {
  DCHECK(socket);
  DCHECK(callback);
  DCHECK(!callback_);

  SetterCallback setter_callback(
      [socket](std::unique_ptr<SocketPosix> accepted_socket) {
        SetSocketDescriptor(socket, std::move(accepted_socket));
      });
  int rv = DoAccept(setter_callback);
  if (rv == ERR_IO_PENDING) callback_ = std::move(callback);
  return rv;
}

int UnixDomainServerSocket::DoAccept(const SetterCallback& setter_callback) {
  DCHECK(!setter_callback.is_null());
  DCHECK(listen_socket_);
  DCHECK(!accept_socket_);

  while (true) {
    int rv = listen_socket_->Accept(
        &accept_socket_,
        CompletionOnceCallback([this, setter_callback](int rv) {
          AcceptCompleted(setter_callback, rv);
        }));
    if (rv != OK) return rv;
    if (AuthenticateAndGetStreamSocket(setter_callback)) return OK;
    // Accept another socket because authentication error should be transparent
    // to the caller.
  }
}

void UnixDomainServerSocket::AcceptCompleted(
    const SetterCallback& setter_callback, int rv) {
  DCHECK(!callback_.is_null());

  if (rv != OK) {
    std::move(callback_).Invoke(rv);
    return;
  }

  if (AuthenticateAndGetStreamSocket(setter_callback)) {
    std::move(callback_).Invoke(OK);
    return;
  }

  // Accept another socket because authentication error should be transparent
  // to the caller.
  rv = DoAccept(setter_callback);
  if (rv != ERR_IO_PENDING) std::move(callback_).Invoke(rv);
}

bool UnixDomainServerSocket::AuthenticateAndGetStreamSocket(
    const SetterCallback& setter_callback) {
  DCHECK(accept_socket_);

  Credentials credentials;
  if (!GetPeerCredentials(accept_socket_->socket_fd(), &credentials) ||
      !auth_callback_.Invoke(credentials)) {
    accept_socket_.reset();
    return false;
  }

  setter_callback.Invoke(std::move(accept_socket_));
  return true;
}

}  // namespace base
