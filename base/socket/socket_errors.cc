// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/socket/socket_errors.h"

#include "base/logging.h"

namespace base {

std::string ErrorToShortString(int error) {
  if (error == OK) return "OK";

  const char* error_string;
  switch (error) {
#define SOCKET_ERROR(label, value) \
  case ERR_##label:                \
    error_string = #label;         \
    break;
#include "base/socket/socket_error_list.h"
#undef SOCKET_ERROR
    default:
      NOTREACHED();
      error_string = "<unknown>";
  }
  return std::string("ERR_") + error_string;
}

}  // namespace base