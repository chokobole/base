// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/socket/address_family.h"

#include "base/logging.h"
#include "base/socket/ip_address.h"
#include "base/socket/sys_addrinfo.h"

namespace base {

AddressFamily GetAddressFamily(const IPAddress& address) {
  if (address.IsIPv4()) {
    return ADDRESS_FAMILY_IPV4;
  } else if (address.IsIPv6()) {
    return ADDRESS_FAMILY_IPV6;
  } else {
    return ADDRESS_FAMILY_UNSPECIFIED;
  }
}

int ConvertAddressFamily(AddressFamily address_family) {
  switch (address_family) {
    case ADDRESS_FAMILY_UNSPECIFIED:
      return AF_UNSPEC;
    case ADDRESS_FAMILY_IPV4:
      return AF_INET;
    case ADDRESS_FAMILY_IPV6:
      return AF_INET6;
  }
  NOTREACHED();
  return AF_UNSPEC;
}

}  // namespace base
