// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MAC_FOUNDATION_UTIL_H_
#define BASE_MAC_FOUNDATION_UTIL_H_

#include <CoreFoundation/CoreFoundation.h>

#include "base/export.h"
#include "base/files/file_path.h"

#if defined(__OBJC__)
#import <Foundation/Foundation.h>
#else   // __OBJC__
class NSString;
#endif  // __OBJC__

namespace base {
namespace mac {

// Converts |path| to an autoreleased NSString. Returns nil if |path| is empty.
BASE_EXPORT NSString* FilePathToNSString(const FilePath& path);

// Converts |str| to a FilePath. Returns an empty path if |str| is nil.
BASE_EXPORT FilePath NSStringToFilePath(NSString* str);

}  // namespace mac
}  // namespace base

#endif  // BASE_MAC_FOUNDATION_UTIL_H_
