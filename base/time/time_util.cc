// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Followings are taken and modified form base/time/time_win.cc
// FILETIME ToFileTime(absl::Time);
// absl::Time FromFileTime(FILETIME);

#include "base/time/time_util.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

#include "absl/base/casts.h"
#include "base/logging.h"

namespace base {

namespace {

#if defined(OS_WIN)
// From MSDN, FILETIME "Contains a 64-bit value representing the number of
// 100-nanosecond intervals since January 1, 1601 (UTC)."
int64_t FileTimeToMicroseconds(FILETIME ft) {
  // Need to bit_cast to fix alignment, then divide by 10 to convert
  // 100-nanoseconds to microseconds. This only works on little-endian
  // machines.
  return absl::bit_cast<int64_t, FILETIME>(ft) / 10;
}

void MicrosecondsToFileTime(int64_t us, FILETIME* ft) {
  DCHECK_GE(us, 0LL) << "Time is less than 0, negative values are not "
                        "representable in FILETIME";

  // Multiply by 10 to convert microseconds to 100-nanoseconds. Bit_cast will
  // handle alignment problems. This only works on little-endian machines.
  *ft = absl::bit_cast<FILETIME, int64_t>(us * 10);
}
#endif

}  // namespace

#if defined(OS_WIN)
FILETIME ToFileTime(absl::Time t) {
  absl::Duration d = absl::time_internal::ToUnixDuration(t);
  if (d == absl::ZeroDuration()) {
    return absl::bit_cast<FILETIME, int64_t>(0);
  }
  if (absl::time_internal::IsInfiniteDuration(d)) {
    FILETIME result;
    result.dwHighDateTime = std::numeric_limits<DWORD>::max();
    result.dwLowDateTime = std::numeric_limits<DWORD>::max();
    return result;
  }
  int64_t us = absl::ToInt64Microseconds(d);
  FILETIME utc_ft;
  MicrosecondsToFileTime(us, &utc_ft);
  return utc_ft;
}

absl::Time FromFileTime(FILETIME ft) {
  if (absl::bit_cast<int64_t, FILETIME>(ft) == 0) return absl::Time();
  if (ft.dwHighDateTime == std::numeric_limits<DWORD>::max() &&
      ft.dwLowDateTime == std::numeric_limits<DWORD>::max())
    return absl::InfiniteFuture();
  return absl::time_internal::FromUnixDuration(
      absl::Microseconds(FileTimeToMicroseconds(ft)));
}
#endif

}  // namespace base