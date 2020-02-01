
// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/win/scoped_handle.h"

#include <windows.h>

namespace base {
namespace win {

// static
bool HandleTraits::CloseHandle(HANDLE handle) {
  if (!::CloseHandle(handle)) CHECK(false);  // CloseHandle failed.
  return true;
}

}  // namespace win
}  // namespace base