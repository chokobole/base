// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// An asynchronous IO operation is not yet complete.  This usually does not
// indicate a fatal error.  Typically this error will be generated as a
// notification to wait for some external notification that the IO operation
// finally completed.
SOCKET_ERROR(IO_PENDING, -1)

// A generic failure occurred.
SOCKET_ERROR(FAILED, -2)

// An operation was aborted (due to user action).
SOCKET_ERROR(ABORTED, -3)

// An argument to the function is incorrect.
SOCKET_ERROR(INVALID_ARGUMENT, -4)

// The handle or file descriptor is invalid.
SOCKET_ERROR(INVALID_HANDLE, -5)

// The file or directory cannot be found.
SOCKET_ERROR(FILE_NOT_FOUND, -6)

// An operation timed out.
SOCKET_ERROR(TIMED_OUT, -7)

// The file is too large.
SOCKET_ERROR(FILE_TOO_BIG, -8)

// An unexpected error.  This may be caused by a programming mistake or an
// invalid assumption.
SOCKET_ERROR(UNEXPECTED, -9)

// Permission to access a resource, other than the network, was denied.
SOCKET_ERROR(ACCESS_DENIED, -10)

// The operation failed because of unimplemented functionality.
SOCKET_ERROR(NOT_IMPLEMENTED, -11)

// There were not enough resources to complete the operation.
SOCKET_ERROR(INSUFFICIENT_RESOURCES, -12)

// Memory allocation failed.
SOCKET_ERROR(OUT_OF_MEMORY, -13)

// The file upload failed because the file's modification time was different
// from the expectation.
SOCKET_ERROR(UPLOAD_FILE_CHANGED, -14)

// The socket is not connected.
SOCKET_ERROR(SOCKET_NOT_CONNECTED, -15)

// The file already exists.
SOCKET_ERROR(FILE_EXISTS, -16)

// The path or file name is too long.
SOCKET_ERROR(FILE_PATH_TOO_LONG, -17)

// Not enough room left on the disk.
SOCKET_ERROR(FILE_NO_SPACE, -18)

// The file has a virus.
SOCKET_ERROR(FILE_VIRUS_INFECTED, -19)

// The client chose to block the request.
SOCKET_ERROR(BLOCKED_BY_CLIENT, -20)

// The network changed.
SOCKET_ERROR(NETWORK_CHANGED, -21)

// The request was blocked by the URL block list configured by the domain
// administrator.
SOCKET_ERROR(BLOCKED_BY_ADMINISTRATOR, -22)

// The socket is already connected.
SOCKET_ERROR(SOCKET_IS_CONNECTED, -23)

// A connection was closed (corresponding to a TCP FIN).
SOCKET_ERROR(CONNECTION_CLOSED, -100)

// A connection was reset (corresponding to a TCP RST).
SOCKET_ERROR(CONNECTION_RESET, -101)

// A connection attempt was refused.
SOCKET_ERROR(CONNECTION_REFUSED, -102)

// A connection timed out as a result of not receiving an ACK for data sent.
// This can include a FIN packet that did not get ACK'd.
SOCKET_ERROR(CONNECTION_ABORTED, -103)

// A connection attempt failed.
SOCKET_ERROR(CONNECTION_FAILED, -104)

// The host name could not be resolved.
SOCKET_ERROR(NAME_NOT_RESOLVED, -105)

// The Internet connection has been lost.
SOCKET_ERROR(INTERNET_DISCONNECTED, -106)

// The IP address or port number is invalid (e.g., cannot connect to the IP
// address 0 or the port 0).
SOCKET_ERROR(ADDRESS_INVALID, -108)

// The IP address is unreachable.  This usually means that there is no route to
// the specified host or network.
SOCKET_ERROR(ADDRESS_UNREACHABLE, -109)

// A connection attempt timed out.
SOCKET_ERROR(CONNECTION_TIMED_OUT, -118)

// Permission to access the network was denied. This is used to distinguish
// errors that were most likely caused by a firewall from other access denied
// errors. See also ERR_ACCESS_DENIED.
SOCKET_ERROR(NETWORK_ACCESS_DENIED, -138)

// The message was too large for the transport.  (for example a UDP message
// which exceeds size threshold).
SOCKET_ERROR(MSG_TOO_BIG, -142)

// Returned when attempting to bind an address that is already in use.
SOCKET_ERROR(ADDRESS_IN_USE, -147)

// Socket ReadIfReady support is not implemented. This error should not be user
// visible, because the normal Read() method is used as a fallback.
SOCKET_ERROR(READ_IF_READY_NOT_IMPLEMENTED, -174)

// No socket buffer space is available.
SOCKET_ERROR(NO_BUFFER_SPACE, -176)