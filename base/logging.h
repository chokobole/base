// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_LOGGING_H_
#define BASE_LOGGING_H_

#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "glog/logging.h"

#define NOTREACHED() CHECK(false)

#if DCHECK_IS_ON()

#define DPLOG(severity) PLOG(severity)

#else  // !DCHECK_IS_ON()

#define DPLOG(severity) \
  static_cast<void>(0), \
      true ? (void)0 : google::LogMessageVoidify() & PLOG(severity)

#endif

#endif  // BASE_LOGGING_H_