// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREAD_THREAD_LOCAL_H_
#define BASE_THREAD_THREAD_LOCAL_H_

#include "base/thread/thread_local_storage.h"

namespace base {

template <typename T>
class ThreadLocalPointer {
 public:
  ThreadLocalPointer() {
    internal::PlatformThreadLocalStorage::AllocTLS(&key_);
  }
  ThreadLocalPointer(const ThreadLocalPointer& other) = delete;
  ThreadLocalPointer& operator=(const ThreadLocalPointer& other) = delete;
  ~ThreadLocalPointer() { internal::PlatformThreadLocalStorage::FreeTLS(key_); }

  T* Get() const {
    return static_cast<T*>(
        internal::PlatformThreadLocalStorage::GetTLSValue(key_));
  }

  void Set(T* ptr) {
    internal::PlatformThreadLocalStorage::SetTLSValue(key_, ptr);
  }

 private:
  internal::PlatformThreadLocalStorage::TLSKey key_;
};

class ThreadLocalBoolean {
 public:
  ThreadLocalBoolean() = default;
  ThreadLocalBoolean(const ThreadLocalBoolean& other) = delete;
  ThreadLocalBoolean& operator=(const ThreadLocalBoolean& other) = delete;
  ~ThreadLocalBoolean() = default;

  bool Get() const { return tlp_.Get() != nullptr; }

  void Set(bool val) { tlp_.Set(val ? this : nullptr); }

 private:
  ThreadLocalPointer<void> tlp_;
};

}  // namespace base

#endif  // BASE_THREAD_THREAD_LOCAL_H_