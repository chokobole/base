// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/socket/winsock_init.h"

#include <winsock2.h>

#include "absl/base/call_once.h"
#include "base/logging.h"

namespace {

class WinsockInit {
 public:
  static void Init() {
    WORD winsock_ver = MAKEWORD(2, 2);
    WSAData wsa_data;
    bool did_init = (WSAStartup(winsock_ver, &wsa_data) == 0);
    if (did_init) {
      DCHECK(wsa_data.wVersion == winsock_ver);

      // The first time WSAGetLastError is called, the delay load helper will
      // resolve the address with GetProcAddress and fixup the import.  If a
      // third party application hooks system functions without correctly
      // restoring the error code, it is possible that the error code will be
      // overwritten during delay load resolution.  The result of the first
      // call may be incorrect, so make sure the function is bound and future
      // results will be correct.
      WSAGetLastError();
    }
  }
};

absl::once_flag g_once;

}  // namespace

namespace base {

void EnsureWinsockInit() { absl::call_once(g_once, &WinsockInit::Init); }

}  // namespace base
