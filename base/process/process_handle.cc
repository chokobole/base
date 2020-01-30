// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/process/process_handle.h"

#include <ostream>

namespace base {

std::ostream& operator<<(std::ostream& os, const UniqueProcId& obj) {
  os << obj.GetUnsafeValue();
  return os;
}

}  // namespace base
