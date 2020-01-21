// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>

#include "base/files/file_path.h"
#include "base/stl_util.h"

namespace base {

#if defined(FILE_PATH_USES_WIN_SEPARATORS)
const char FilePath::kSeparators[] = "\\/";
#else   // FILE_PATH_USES_WIN_SEPARATORS
const char FilePath::kSeparators[] = "/";
#endif  // FILE_PATH_USES_WIN_SEPARATORS

const size_t FilePath::kSeparatorsLength = size(kSeparators);

const char FilePath::kCurrentDirectory[] = ".";
const char FilePath::kParentDirectory[] = "..";

const char FilePath::kExtensionSeparator = '.';

}  // namespace base
