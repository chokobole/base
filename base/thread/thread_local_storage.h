// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREAD_THREAD_LOCAL_STORAGE_H_
#define BASE_THREAD_THREAD_LOCAL_STORAGE_H_

#include <stdint.h>

#include "base/build_config.h"
#include "base/export.h"

#if defined(OS_POSIX)
#include <pthread.h>
#endif

namespace base {

namespace internal {

// WARNING: You should *NOT* use this class directly.
// PlatformThreadLocalStorage is a low-level abstraction of the OS's TLS
// interface. Instead, you should use one of the following:
// * ThreadLocalBoolean (from thread_local.h) for booleans.
// * ThreadLocalPointer (from thread_local.h) for pointers.
class BASE_EXPORT PlatformThreadLocalStorage {
 public:
#if defined(OS_WIN)
  typedef unsigned long TLSKey;
#elif defined(OS_POSIX)
  typedef pthread_key_t TLSKey;
#endif

  // The following methods need to be supported on each OS platform, so that
  // the Chromium ThreadLocalStore functionality can be constructed.
  // Chromium will use these methods to acquire a single OS slot, and then use
  // that to support a much larger number of Chromium slots (independent of the
  // OS restrictions).
  // The following returns true if it successfully is able to return an OS
  // key in |key|.
  static bool AllocTLS(TLSKey* key);
  // Note: FreeTLS() doesn't have to be called, it is fine with this leak, OS
  // might not reuse released slot, you might just reset the TLS value with
  // SetTLSValue().
  static void FreeTLS(TLSKey key);
  static void SetTLSValue(TLSKey key, void* value);
  static void* GetTLSValue(TLSKey key);
};

}  // namespace internal
}  // namespace base

#endif  // BASE_THREADING_THREAD_LOCAL_STORAGE_H_
