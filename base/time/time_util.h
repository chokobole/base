// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TME_TIME_UTIL_H_
#define BASE_TME_TIME_UTIL_H_

#include "absl/time/time.h"
#include "base/build_config.h"
#include "base/export.h"

#if defined(OS_WIN)
#include "base/win/windows_types.h"
#endif

namespace base {

#if defined(OS_MACOSX) && !defined(OS_IOS)
BASE_EXPORT absl::Time FromMachAbsoluteTime(uint64_t mach_absolute_time);
#endif

#if defined(OS_WIN)
BASE_EXPORT FILETIME ToFileTime(absl::Time t);
BASE_EXPORT absl::Time FromFileTime(FILETIME t);
#endif

}  // namespace base

#endif  // BASE_TME_TIME_UTIL_H_