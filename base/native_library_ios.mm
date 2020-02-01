// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/native_library.h"

#include "base/logging.h"
#include "base/strings/string_util.h"

namespace base {

std::string NativeLibraryLoadError::ToString() const { return message; }

NativeLibrary LoadNativeLibraryWithOptions(const base::FilePath& library_path,
                                           const NativeLibraryOptions& options,
                                           NativeLibraryLoadError* error) {
  NOTIMPLEMENTED();
  if (error) error->message = "Not implemented.";
  return nullptr;
}

void UnloadNativeLibrary(NativeLibrary library) {
  NOTIMPLEMENTED();
  DCHECK(!library);
}

void* GetFunctionPointerFromNativeLibrary(NativeLibrary library, absl::string_view name) {
  NOTIMPLEMENTED();
  return nullptr;
}

std::string GetNativeLibraryName(absl::string_view name) {
  DCHECK(IsStringASCII(name));
  return std::string(name);
}

}  // namespace base
