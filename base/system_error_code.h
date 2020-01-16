// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Followings are taken and modified form base/logging.h
// SystemErrorCode
// SystemErrorCode GetLastSystemErrorCode()
// std::string SystemErrorCodeToString(SystemErrorCode error_code)

#ifndef BASE_SYSTEM_ERROR_CODE_H_
#define BASE_SYSTEM_ERROR_CODE_H_

#include <string>

#include "base/build_config.h"
#include "base/export.h"

namespace base {

#if defined(OS_WIN)
typedef unsigned long SystemErrorCode;
#elif defined(OS_POSIX)
typedef int SystemErrorCode;
#endif

// Alias for ::GetLastError() on Windows and errno on POSIX. Avoids having to
// pull in windows.h just for GetLastError() and DWORD.
BASE_EXPORT SystemErrorCode GetLastSystemErrorCode();
BASE_EXPORT std::string SystemErrorCodeToString(SystemErrorCode error_code);

}  // namespace base

#endif  // BASE_SYSTEM_ERROR_CODE_H_