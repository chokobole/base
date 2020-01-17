// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains utility functions for dealing with the local
// filesystem.

#ifndef BASE_FILES_FILE_UTIL_H_
#define BASE_FILES_FILE_UTIL_H_

#include "base/build_config.h"
#include "base/export.h"

namespace base {

// Sets the given |fd| to non-blocking mode.
// Returns true if it was able to set it in the non-blocking mode, otherwise
// false.
BASE_EXPORT bool SetNonBlocking(int fd);

#if defined(OS_POSIX)

// Sets the given |fd| to close-on-exec mode.
// Returns true if it was able to set it in the close-on-exec mode, otherwise
// false.
BASE_EXPORT bool SetCloseOnExec(int fd);

#endif

}  // namespace base

#endif  // BASE_FILES_FILE_UTIL_H_
