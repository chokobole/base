// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_COMPLETION_ONCE_CALLBACK_H_
#define BASE_COMPLETION_ONCE_CALLBACK_H_

#include <stdint.h>

#include "base/callback.h"

namespace base {

// A OnceCallback specialization that takes a single int parameter. Usually this
// is used to report a byte count or network error code.
using CompletionOnceCallback = OnceCallback<void(int)>;

// 64bit version of the OnceCallback specialization that takes a single int64_t
// parameter. Usually this is used to report a file offset, size or network
// error code.
using Int64CompletionOnceCallback = OnceCallback<void(int64_t)>;

}  // namespace base

#endif  // BASE_COMPLETION_ONCE_CALLBACK_H_
