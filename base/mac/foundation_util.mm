// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/mac/foundation_util.h"

#include "base/files/file_path.h"

namespace base {
namespace mac {

NSString* FilePathToNSString(const FilePath& path) {
  if (path.empty()) return nil;
  return @(path.value().c_str());  // @() does UTF8 conversion.
}

FilePath NSStringToFilePath(NSString* str) {
  if (![str length]) return FilePath();
  return FilePath([str fileSystemRepresentation]);
}

}  // namespace mac
}  // namespace base
