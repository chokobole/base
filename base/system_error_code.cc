// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Followings are taken and modified form base/logging.cc
// SystemErrorCode GetLastSystemErrorCode()
// std::string SystemErrorCodeToString(SystemErrorCode error_code)

#include "base/system_error_code.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

#include "absl/strings/ascii.h"
#include "absl/strings/str_format.h"
#include "base/logging.h"
#include "base/stl_util.h"

namespace base {

#if defined(OS_WIN)
// This has already been defined in the header, but defining it again as DWORD
// ensures that the type used in the header is equivalent to DWORD. If not,
// the redefinition is a compile error.
typedef DWORD SystemErrorCode;
#endif

SystemErrorCode GetLastSystemErrorCode() {
#if defined(OS_WIN)
  return ::GetLastError();
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
  return errno;
#endif
}

std::string SystemErrorCodeToString(SystemErrorCode error_code) {
#if defined(OS_WIN)
  const int kErrorMessageBufferSize = 256;
  char msgbuf[kErrorMessageBufferSize];
  DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
  DWORD len = FormatMessageA(flags, nullptr, error_code, 0, msgbuf,
                             size(msgbuf), nullptr);
  if (len) {
    // Messages returned by system end with line breaks.
    std::string msg(msgbuf);
    absl::StripAsciiWhitespace(&msg);
    return msg + absl::StrFormat(" (0x%lX)", error_code);
  }
  return absl::StrFormat("Error (0x%lX) while retrieving error. (0x%lX)",
                         GetLastError(), error_code);
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
  return google::StrError(error_code) + absl::StrFormat(" (%d)", error_code);
#endif  // defined(OS_WIN)
}

}  // namespace base