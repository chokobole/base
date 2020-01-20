// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_LOGGING_H_
#define BASE_LOGGING_H_

#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "glog/logging.h"

#define NOTREACHED() CHECK(false)

#define VPLOG(verboselevel) PLOG_IF(INFO, VLOG_IS_ON(verboselevel))

#if DCHECK_IS_ON()

#define DPLOG(severity) PLOG(severity)

#define DVPLOG(verboselevel) VPLOG(verboselevel)

#else  // !DCHECK_IS_ON()

#define DPLOG(severity) \
  static_cast<void>(0), \
      true ? (void)0 : google::LogMessageVoidify() & PLOG(severity)

#define DVPLOG(verboselevel)                                \
  static_cast<void>(0), (true || !VLOG_IS_ON(verboselevel)) \
                            ? (void)0                       \
                            : google::LogMessageVoidify() & VPLOG(INFO)

#endif

#if defined(COMPILER_GCC)
// On Linux, with GCC, we can use __PRETTY_FUNCTION__ to get the demangled name
// of the current function in the NOTIMPLEMENTED message.
#define NOTIMPLEMENTED_MSG "Not implemented reached in " << __PRETTY_FUNCTION__
#else
#define NOTIMPLEMENTED_MSG "NOT IMPLEMENTED"
#endif

#define NOTIMPLEMENTED() DLOG(ERROR) << NOTIMPLEMENTED_MSG

#endif  // BASE_LOGGING_H_