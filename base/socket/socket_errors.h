// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Modified from net/base/net_errors.h

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_ERRORS_H_
#define BASE_SOCKET_ERRORS_H_

#include "base/export.h"
#include "base/system_error_code.h"

namespace base {

enum SocketErrorCode {
  OK = 0,
  // An asynchronous IO operation is not yet complete.  This usually does not
  // indicate a fatal error.  Typically this error will be generated as a
  // notification to wait for some external notification that the IO operation
  // finally completed.
  ERR_IO_PENDING = -1,

  // A generic failure occurred.
  ERR_FAILED = -2,

  // An operation was aborted (due to user action).
  ERR_ABORTED = -3,

  // An argument to the function is incorrect.
  ERR_INVALID_ARGUMENT = -4,

  // The handle or file descriptor is invalid.
  ERR_INVALID_HANDLE = -5,

  // The file or directory cannot be found.
  ERR_FILE_NOT_FOUND = -6,

  // An operation timed out.
  ERR_TIMED_OUT = -7,

  // The file is too large.
  ERR_FILE_TOO_BIG = -8,

  // An unexpected error.  This may be caused by a programming mistake or an
  // invalid assumption.
  ERR_UNEXPECTED = -9,

  // Permission to access a resource, other than the network, was denied.
  ERR_ACCESS_DENIED = -10,

  // The operation failed because of unimplemented functionality.
  ERR_NOT_IMPLEMENTED = -11,

  // There were not enough resources to complete the operation.
  ERR_INSUFFICIENT_RESOURCES = -12,

  // Memory allocation failed.
  ERR_OUT_OF_MEMORY = -13,

  // The file upload failed because the file's modification time was different
  // from the expectation.
  ERR_UPLOAD_FILE_CHANGED = -14,

  // The socket is not connected.
  ERR_SOCKET_NOT_CONNECTED = -15,

  // The file already exists.
  ERR_FILE_EXISTS = -16,

  // The path or file name is too long.
  ERR_FILE_PATH_TOO_LONG = -17,

  // Not enough room left on the disk.
  ERR_FILE_NO_SPACE = -18,

  // The file has a virus.
  ERR_FILE_VIRUS_INFECTED = -19,

  // The client chose to block the request.
  ERR_BLOCKED_BY_CLIENT = -20,

  // The network changed.
  ERR_NETWORK_CHANGED = -21,

  // The socket is already connected.
  ERR_SOCKET_IS_CONNECTED = -23,

  // A connection was closed (corresponding to a TCP FIN).
  ERR_CONNECTION_CLOSED = -100,

  // A connection was reset (corresponding to a TCP RST).
  ERR_CONNECTION_RESET = -101,

  // A connection attempt was refused.
  ERR_CONNECTION_REFUSED = -102,

  // A connection timed out as a result of not receiving an ACK for data sent.
  // This can include a FIN packet that did not get ACK'd.
  ERR_CONNECTION_ABORTED = -103,

  // A connection attempt failed.
  ERR_CONNECTION_FAILED = -104,

  // The host name could not be resolved.
  ERR_NAME_NOT_RESOLVED = -105,

  // The Internet connection has been lost.
  ERR_INTERNET_DISCONNECTED = -106,

  // The IP address or port number is invalid (e.g., cannot connect to the IP
  // address 0 or the port 0).
  ERR_ADDRESS_INVALID = -108,

  // The IP address is unreachable.  This usually means that there is no route
  // to the specified host or network.
  ERR_ADDRESS_UNREACHABLE = -109,

  // A connection attempt timed out.
  ERR_CONNECTION_TIMED_OUT = -118,

  // Permission to access the network was denied. This is used to distinguish
  // errors that were most likely caused by a firewall from other access denied
  // errors. See also ERR_ACCESS_DENIED.
  ERR_NETWORK_ACCESS_DENIED = -138,

  // The message was too large for the transport.  (for example a UDP message
  // which exceeds size threshold).
  ERR_MSG_TOO_BIG = -142,

  // Returned when attempting to bind an address that is already in use.
  ERR_ADDRESS_IN_USE = -147,

  // Socket ReadIfReady support is not implemented. This error should not be
  // user visible, because the normal Read() method is used as a fallback.
  ERR_READ_IF_READY_NOT_IMPLEMENTED = -174,

  // No socket buffer space is available.
  ERR_NO_BUFFER_SPACE = -176,
};

// Map system error code to Error.
BASE_EXPORT SocketErrorCode MapSystemError(SystemErrorCode os_error);

}  // namespace base

#endif  // BASE_SOCKET_ERRORS_H_