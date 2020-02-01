// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/native_library.h"

#include <dlfcn.h>

#include "absl/strings/substitute.h"
#include "base/logging.h"
#include "base/strings/string_util.h"

namespace base {

std::string NativeLibraryLoadError::ToString() const { return message; }

NativeLibrary LoadNativeLibraryWithOptions(const FilePath& library_path,
                                           const NativeLibraryOptions& options,
                                           NativeLibraryLoadError* error) {
  // We deliberately do not use RTLD_DEEPBIND by default.  For the history why,
  // please refer to the bug tracker.  Some useful bug reports to read include:
  // http://crbug.com/17943, http://crbug.com/17557, http://crbug.com/36892,
  // and http://crbug.com/40794.
  int flags = RTLD_LAZY;
#if defined(OS_ANDROID) || !defined(RTLD_DEEPBIND)
  // Certain platforms don't define RTLD_DEEPBIND. Android dlopen() requires
  // further investigation, as it might vary across versions. Crash here to
  // warn developers that they're trying to rely on uncertain behavior.
  CHECK(!options.prefer_own_symbols);
#else
  if (options.prefer_own_symbols) flags |= RTLD_DEEPBIND;
#endif
  void* dl = dlopen(library_path.value().c_str(), flags);
  if (!dl && error) error->message = dlerror();

  return dl;
}

void UnloadNativeLibrary(NativeLibrary library) {
  int ret = dlclose(library);
  if (ret < 0) {
    DLOG(ERROR) << "dlclose failed: " << dlerror();
    NOTREACHED();
  }
}

void* GetFunctionPointerFromNativeLibrary(NativeLibrary library,
                                          absl::string_view name) {
  return dlsym(library, name.data());
}

std::string GetNativeLibraryName(absl::string_view name) {
  DCHECK(IsStringASCII(name));
  return absl::Substitute("lib$0.so", name);
}

std::string GetLoadableModuleName(absl::string_view name) {
  return GetNativeLibraryName(name);
}

}  // namespace base
