// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_TCP_SOCKET_WIN_H_
#define BASE_SOCKET_TCP_SOCKET_WIN_H_

#include <stdint.h>
#include <winsock2.h>

#include <memory>

#include "base/compiler_specific.h"
#include "base/win/object_watcher.h"
#include "base/socket/address_family.h"
#include "base/completion_once_callback.h"
#include "base/export.h"
#include "base/socket/socket_descriptor.h"

namespace base {

class AddressList;
class IOBuffer;
class IPEndPoint;

class BASE_EXPORT TCPSocketWin : public win::ObjectWatcher::Delegate {
 public:
  TCPSocketWin();
  TCPSocketWin(const TCPSocketWin& other) = delete;
  TCPSocketWin& operator=(const TCPSocketWin& other) = delete;
  ~TCPSocketWin() override;

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

  int Bind(const IPEndPoint& address);

  int Listen(int backlog);
  int Accept(std::unique_ptr<TCPSocketWin>* socket,
             IPEndPoint* address,
             CompletionOnceCallback callback);

  int Connect(const IPEndPoint& address, CompletionOnceCallback callback);
  bool IsConnected() const;
  bool IsConnectedAndIdle() const;

  // Multiple outstanding requests are not supported.
  // Full duplex mode (reading and writing at the same time) is supported.
  int Read(std::shared_ptr<IOBuffer> buf, int buf_len, CompletionOnceCallback callback);
  int ReadIfReady(std::shared_ptr<IOBuffer> buf, int buf_len, CompletionOnceCallback callback);
  int CancelReadIfReady();
  int Write(std::shared_ptr<IOBuffer> buf,
            int buf_len,
            CompletionOnceCallback callback);

  int GetLocalAddress(IPEndPoint* address) const;
  int GetPeerAddress(IPEndPoint* address) const;

  // Sets various socket options.
  // The commonly used options for server listening sockets:
  // - SetExclusiveAddrUse().
  int SetDefaultOptionsForServer();
  // The commonly used options for client sockets and accepted sockets:
  // - SetNoDelay(true);
  // - SetKeepAlive(true, 45).
  void SetDefaultOptionsForClient();
  int SetExclusiveAddrUse();
  int SetReceiveBufferSize(int32_t size);
  int SetSendBufferSize(int32_t size);
  bool SetKeepAlive(bool enable, int delay);
  bool SetNoDelay(bool no_delay);

  // Gets the estimated RTT. Returns false if the RTT is
  // unavailable. May also return false when estimated RTT is 0.
  bool GetEstimatedRoundTripTime(TimeDelta* out_rtt) const
      WARN_UNUSED_RESULT;

  void Close();

  bool IsValid() const { return socket_ != INVALID_SOCKET; }

 private:
  class Core;

  // ObjectWatcher::Delegate implementation.
  void OnObjectSignaled(HANDLE object) override;

  int AcceptInternal(std::unique_ptr<TCPSocketWin>* socket,
                     IPEndPoint* address);

  int DoConnect();

  void RetryRead(int rv);
  void DidCompleteConnect();
  void DidCompleteWrite();
  void DidSignalRead();

  SOCKET socket_;

  HANDLE accept_event_;
  win::ObjectWatcher accept_watcher_;

  std::unique_ptr<TCPSocketWin>* accept_socket_;
  IPEndPoint* accept_address_;
  CompletionOnceCallback accept_callback_;

  // The various states that the socket could be in.
  bool waiting_connect_;
  bool waiting_read_;
  bool waiting_write_;

  // The core of the socket that can live longer than the socket itself. We pass
  // resources to the Windows async IO functions and we have to make sure that
  // they are not destroyed while the OS still references them.
  std::shared_ptr<Core> core_;

  // External callback; called when connect or read is complete.
  CompletionOnceCallback read_callback_;

  // Non-null if a ReadIfReady() is to be completed asynchronously. This is an
  // external callback if user used ReadIfReady() instead of Read(), but a
  // wrapped callback on top of RetryRead() if Read() is used.
  CompletionOnceCallback read_if_ready_callback_;

  // External callback; called when write is complete.
  CompletionOnceCallback write_callback_;

  std::unique_ptr<IPEndPoint> peer_address_;
};

}  // namespace base

#endif  // BASE_SOCKET_TCP_SOCKET_WIN_H_
