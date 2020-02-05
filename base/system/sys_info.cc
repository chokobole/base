// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/system/sys_info.h"

#include <algorithm>

#include "absl/time/clock.h"
#include "base/build_config.h"

namespace base {

// static
int64_t SysInfo::AmountOfPhysicalMemory() {
  return AmountOfPhysicalMemoryImpl();
}

// static
int64_t SysInfo::AmountOfAvailablePhysicalMemory() {
  return AmountOfAvailablePhysicalMemoryImpl();
}

#if !defined(OS_MACOSX) && !defined(OS_ANDROID)
std::string SysInfo::HardwareModelName() { return std::string(); }
#endif

// static
absl::Duration SysInfo::Uptime() {
  return absl::time_internal::ToUnixDuration(absl::Now());
}

}  // namespace base
