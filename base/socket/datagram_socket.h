// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_DATAGRAM_SOCKET_H_
#define BASE_SOCKET_DATAGRAM_SOCKET_H_

#include "base/export.h"

namespace base {

class IPEndPoint;

// A datagram socket is an interface to a protocol which exchanges
// datagrams, like UDP.
class BASE_EXPORT DatagramSocket {
 public:
  // Type of source port binding to use.
  enum BindType {
    RANDOM_BIND,
    DEFAULT_BIND,
  };

  virtual ~DatagramSocket() {}

  // Close the socket.
  virtual void Close() = 0;

  // Copy the remote udp address into |address| and return a network error code.
  virtual int GetPeerAddress(IPEndPoint* address) const = 0;

  // Copy the local udp address into |address| and return a network error code.
  // (similar to getsockname)
  virtual int GetLocalAddress(IPEndPoint* address) const = 0;

  // Switch to use non-blocking IO. Must be called right after construction and
  // before other calls.
  virtual void UseNonBlockingIO() = 0;

  // Requests that packets sent by this socket not be fragment, either locally
  // by the host, or by routers (via the DF bit in the IPv4 packet header).
  // May not be supported by all platforms. Returns a return a network error
  // code if there was a problem, but the socket will still be usable. Can not
  // return ERR_IO_PENDING.
  virtual int SetDoNotFragment() = 0;

  // If |confirm| is true, then the MSG_CONFIRM flag will be passed to
  // subsequent writes if it's supported by the platform.
  virtual void SetMsgConfirm(bool confirm) = 0;
};

}  // namespace base

#endif  // BASE_SOCKET_DATAGRAM_SOCKET_H_
