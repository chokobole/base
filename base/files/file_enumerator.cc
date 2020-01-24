// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/file_enumerator.h"

namespace base {

FileEnumerator::FileInfo::~FileInfo() = default;

bool FileEnumerator::ShouldSkip(const FilePath& path) {
  std::string basename = path.BaseName().value();
  return basename == "." ||
         (basename == ".." && !(INCLUDE_DOT_DOT & file_type_));
}

bool FileEnumerator::IsTypeMatched(bool is_dir) const {
  return (file_type_ &
          (is_dir ? FileEnumerator::DIRECTORIES : FileEnumerator::FILES)) != 0;
}

}  // namespace base
