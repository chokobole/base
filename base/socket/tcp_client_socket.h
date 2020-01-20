// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_TCP_CLIENT_SOCKET_H_
#define BASE_SOCKET_TCP_CLIENT_SOCKET_H_

#include <stdint.h>

#include <memory>

#include "base/build_config.h"
#include "base/compiler_specific.h"
#include "base/completion_once_callback.h"
#include "base/export.h"
#include "base/socket/address_list.h"
#include "base/socket/socket_descriptor.h"
#include "base/socket/stream_socket.h"
#include "base/socket/tcp_socket.h"
#include "base/socket/transport_client_socket.h"

namespace base {

class IPEndPoint;

// A client socket that uses TCP as the transport layer.
class BASE_EXPORT TCPClientSocket : public TransportClientSocket {
 public:
  // The IP address(es) and port number to connect to.  The TCP socket will try
  // each IP address in the list until it succeeds in establishing a
  // connection.
  explicit TCPClientSocket(const AddressList& addresses);

  // Adopts the given, connected socket and then acts as if Connect() had been
  // called. This function is used by TCPServerSocket and for testing.
  TCPClientSocket(std::unique_ptr<TCPSocket> connected_socket,
                  const IPEndPoint& peer_address);

  TCPClientSocket(const TCPClientSocket& other) = delete;
  TCPClientSocket& operator=(const TCPClientSocket& other) = delete;

  // Creates a TCPClientSocket from a bound-but-not-connected socket.
  static std::unique_ptr<TCPClientSocket> CreateFromBoundSocket(
      std::unique_ptr<TCPSocket> bound_socket, const AddressList& addresses,
      const IPEndPoint& bound_address);

  ~TCPClientSocket() override;

  // TransportClientSocket implementation.
  int Bind(const IPEndPoint& address) override;
  bool SetKeepAlive(bool enable, int delay) override;
  bool SetNoDelay(bool no_delay) override;

  // StreamSocket implementation.
  void SetBeforeConnectCallback(
      const BeforeConnectCallback& before_connect_callback) override;
  int Connect(CompletionOnceCallback callback) override;
  void Disconnect() override;
  bool IsConnected() const override;
  bool IsConnectedAndIdle() const override;
  int GetPeerAddress(IPEndPoint* address) const override;
  int GetLocalAddress(IPEndPoint* address) const override;
  bool WasEverUsed() const override;
  int64_t GetTotalReceivedBytes() const override;

  // Socket implementation.
  // Multiple outstanding requests are not supported.
  // Full duplex mode (reading and writing at the same time) is supported.
  int Read(std::shared_ptr<IOBuffer> buf, int buf_len,
           CompletionOnceCallback callback) override;
  int ReadIfReady(std::shared_ptr<IOBuffer> buf, int buf_len,
                  CompletionOnceCallback callback) override;
  int CancelReadIfReady() override;
  int Write(std::shared_ptr<IOBuffer> buf, int buf_len,
            CompletionOnceCallback callback) override;
  int SetReceiveBufferSize(int32_t size) override;
  int SetSendBufferSize(int32_t size) override;

 private:
  // State machine for connecting the socket.
  enum ConnectState {
    CONNECT_STATE_CONNECT,
    CONNECT_STATE_CONNECT_COMPLETE,
    CONNECT_STATE_NONE,
  };

  // Main constructor. |socket| must be non-null. |current_address_index| is the
  // address index in |addresses| of the server |socket| is connected to, or -1
  // if not connected. |bind_address|, if present, is the address |socket| is
  // bound to.
  TCPClientSocket(std::unique_ptr<TCPSocket> socket,
                  const AddressList& addresses, int current_address_index,
                  std::unique_ptr<IPEndPoint> bind_address);

  // A helper method shared by Read() and ReadIfReady(). If |read_if_ready| is
  // set to true, ReadIfReady() will be used instead of Read().
  int ReadCommon(std::shared_ptr<IOBuffer> buf, int buf_len,
                 const CompletionOnceCallback callback, bool read_if_ready);

  // State machine used by Connect().
  int DoConnectLoop(int result);
  int DoConnect();
  int DoConnectComplete(int result);

  // Calls the connect method of |socket_|. Used in tests, to ensure a socket
  // never connects.
  virtual int ConnectInternal(const IPEndPoint& endpoint);

  // Helper used by Disconnect(), which disconnects minus resetting
  // current_address_index_ and bind_address_.
  void DoDisconnect();

  void DidCompleteConnect(int result);
  void DidCompleteRead(int result);
  void DidCompleteWrite(int result);
  void DidCompleteReadWrite(CompletionOnceCallback callback, int result);

  int OpenSocket(AddressFamily family);

  std::unique_ptr<TCPSocket> socket_;

  // Local IP address and port we are bound to. Set to NULL if Bind()
  // wasn't called (in that case OS chooses address/port).
  std::unique_ptr<IPEndPoint> bind_address_;

  // The list of addresses we should try in order to establish a connection.
  AddressList addresses_;

  // Where we are in above list. Set to -1 if uninitialized.
  int current_address_index_;

  // External callbacks; called when corresponding operations are complete.
  // Cleared when no such operation is pending.
  CompletionOnceCallback connect_callback_;
  CompletionOnceCallback read_callback_;
  CompletionOnceCallback write_callback_;

  // The next state for the Connect() state machine.
  ConnectState next_connect_state_;

  // This socket was previously disconnected and has not been re-connected.
  bool previously_disconnected_;

  // Total number of bytes received by the socket.
  int64_t total_received_bytes_;

  BeforeConnectCallback before_connect_callback_;

  bool was_ever_used_;

  // Set to true if the socket was disconnected due to entering suspend mode.
  // Once set, read/write operations return ERR_NETWORK_IO_SUSPENDED, until
  // Connect() or Disconnect() is called.
  bool was_disconnected_on_suspend_;
};

}  // namespace base

#endif  // BASE_SOCKET_TCP_CLIENT_SOCKET_H_
