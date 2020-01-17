// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SOCKET_ADDRESS_LIST_H_
#define BASE_SOCKET_ADDRESS_LIST_H_

#include <vector>

#include "base/export.h"
#include "base/socket/ip_endpoint.h"

struct addrinfo;

namespace base {

class BASE_EXPORT AddressList : public std::vector<IPEndPoint> {
 public:
  AddressList();
  AddressList(const std::vector<IPEndPoint>& other);
  AddressList(std::vector<IPEndPoint>&& other);
  AddressList(const AddressList& other);
  AddressList(AddressList&& other);
  AddressList& operator=(const std::vector<IPEndPoint>& other);
  AddressList& operator=(std::vector<IPEndPoint>&& other);
  AddressList& operator=(const AddressList& other);
  AddressList& operator=(AddressList&& other);
  ~AddressList();

  // Creates an address list for a single IP literal.
  explicit AddressList(const IPEndPoint& endpoint);

  static AddressList CreateFromIPAddress(const IPAddress& address,
                                         uint16_t port);

  static AddressList CreateFromIPAddressList(const IPAddressList& addresses);

  // Copies the data from |head| and the chained list into an AddressList.
  static AddressList CreateFromAddrinfo(const struct addrinfo* head);

  // Returns a copy of |list| with port on each element set to |port|.
  static AddressList CopyWithPort(const AddressList& list, uint16_t port);

  // Deduplicates the stored addresses while otherwise preserving their order.
  void Deduplicate();
};

}  // namespace base

#endif  // BASE_SOCKET_ADDRESS_LIST_H_
