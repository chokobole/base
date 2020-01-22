// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_WIN_SCOPED_HANDLE_H_
#define BASE_WIN_SCOPED_HANDLE_H_

#include "base/compiler_specific.h"
#include "base/logging.h"
#include "base/win/windows_types.h"

namespace base {
namespace win {

// Generic wrapper for raw handles that takes care of closing handles
// automatically. The class interface follows the style of
// the ScopedFILE class with two additions:
//   - IsValid() method can tolerate multiple invalid handle values such as NULL
//     and INVALID_HANDLE_VALUE (-1) for Win32 handles.
//   - Set() (and the constructors and assignment operators that call it)
//     preserve the Windows LastError code. This ensures that GetLastError() can
//     be called after stashing a handle in a GenericScopedHandle object. Doing
//     this explicitly is necessary because of bug 528394 and VC++ 2015.
template <class Traits>
class GenericScopedHandle {
 public:
  using Handle = typename Traits::Handle;

  GenericScopedHandle() : handle_(Traits::NullHandle()) {}

  explicit GenericScopedHandle(Handle handle) : handle_(Traits::NullHandle()) {
    Set(handle);
  }

  GenericScopedHandle(const GenericScopedHandle& other) = delete;
  GenericScopedHandle& operator=(const GenericScopedHandle& other) = delete;

  GenericScopedHandle(GenericScopedHandle&& other)
      : handle_(Traits::NullHandle()) {
    Set(other.Take());
  }

  ~GenericScopedHandle() { Close(); }

  bool IsValid() const { return Traits::IsHandleValid(handle_); }

  GenericScopedHandle& operator=(GenericScopedHandle&& other) {
    DCHECK_NE(this, &other);
    Set(other.Take());
    return *this;
  }

  void Set(Handle handle) {
    if (handle_ != handle) {
      // Preserve old LastError to avoid bug 528394.
      auto last_error = ::GetLastError();
      Close();

      if (Traits::IsHandleValid(handle)) {
        handle_ = handle;
      }
      ::SetLastError(last_error);
    }
  }

  Handle Get() const { return handle_; }

  // Transfers ownership away from this object.
  Handle Take() WARN_UNUSED_RESULT {
    Handle temp = handle_;
    handle_ = Traits::NullHandle();
    return temp;
  }

  // Explicitly closes the owned handle.
  void Close() {
    if (Traits::IsHandleValid(handle_)) {
      Traits::CloseHandle(handle_);
      handle_ = Traits::NullHandle();
    }
  }

 private:
  Handle handle_;
};

// The traits class for Win32 handles that can be closed via CloseHandle() API.
class HandleTraits {
 public:
  using Handle = HANDLE;

  HandleTraits() = delete;
  HandleTraits(const HandleTraits& other) = delete;
  HandleTraits& operator=(const HandleTraits& other) = delete;

  // Closes the handle.
  static bool CloseHandle(HANDLE handle) {
    if (!::CloseHandle(handle)) CHECK(false);  // CloseHandle failed.
    return true;
  }

  // Returns true if the handle value is valid.
  static bool IsHandleValid(HANDLE handle) {
    return handle != nullptr && handle != INVALID_HANDLE_VALUE;
  }

  // Returns NULL handle value.
  static HANDLE NullHandle() { return nullptr; }
};

using ScopedHandle = GenericScopedHandle<HandleTraits>;

}  // namespace win
}  // namespace base

#endif  // BASE_WIN_SCOPED_HANDLE_H_
