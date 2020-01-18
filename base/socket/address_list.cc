// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/socket/address_list.h"

#include <utility>

#include "base/containers/flat_map.h"
#include "base/logging.h"
#include "base/socket/sys_addrinfo.h"

namespace base {

AddressList::AddressList() = default;

AddressList::AddressList(const std::vector<IPEndPoint>& other)
    : std::vector<IPEndPoint>(other) {}

AddressList::AddressList(std::vector<IPEndPoint>&& other)
    : std::vector<IPEndPoint>(std::move(other)) {}

AddressList::AddressList(const AddressList& other) = default;

AddressList::AddressList(AddressList&& other) = default;

AddressList& AddressList::operator=(const AddressList& other) = default;

AddressList& AddressList::operator=(AddressList&& other) = default;

AddressList& AddressList::operator=(const std::vector<IPEndPoint>& other) {
  std::vector<IPEndPoint>::operator=(other);
  return *this;
}

AddressList& AddressList::operator=(std::vector<IPEndPoint>&& other) {
  std::vector<IPEndPoint>::operator=(std::move(other));
  return *this;
}

AddressList::~AddressList() = default;

AddressList::AddressList(const IPEndPoint& endpoint) { push_back(endpoint); }

// static
AddressList AddressList::CreateFromIPAddress(const IPAddress& address,
                                             uint16_t port) {
  return AddressList(IPEndPoint(address, port));
}

// static
AddressList AddressList::CreateFromIPAddressList(
    const IPAddressList& addresses) {
  AddressList list;
  list.reserve(addresses.size());
  for (auto iter = addresses.begin(); iter != addresses.end(); ++iter) {
    list.push_back(IPEndPoint(*iter, 0));
  }
  return list;
}

// static
AddressList AddressList::CreateFromAddrinfo(const struct addrinfo* head) {
  DCHECK(head);
  AddressList list;
  for (const struct addrinfo* ai = head; ai; ai = ai->ai_next) {
    IPEndPoint ipe;
    // NOTE: Ignoring non-INET* families.
    if (ipe.FromSockAddr(ai->ai_addr, static_cast<socklen_t>(ai->ai_addrlen)))
      list.push_back(ipe);
    else
      DLOG(WARNING) << "Unknown family found in addrinfo: " << ai->ai_family;
  }
  return list;
}

// static
AddressList AddressList::CopyWithPort(const AddressList& list, uint16_t port) {
  AddressList out;
  out.reserve(out.size());
  for (size_t i = 0; i < list.size(); ++i)
    out.push_back(IPEndPoint(list[i].address(), port));
  return out;
}

void AddressList::Deduplicate() {
  if (size() > 1) {
    std::vector<std::pair<IPEndPoint, int>> make_me_into_a_map(size());
    for (auto& addr : *this) make_me_into_a_map.emplace_back(addr, 0);
    flat_map<IPEndPoint, int> inserted(std::move(make_me_into_a_map));

    AddressList deduplicated_addresses;
    deduplicated_addresses.reserve(inserted.size());
    for (const auto& addr : *this) {
      int& count = inserted[addr];
      if (!count) {
        deduplicated_addresses.push_back(addr);
        ++count;
      }
    }
    swap(deduplicated_addresses);
  }
}

}  // namespace base
