// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"
#include "base/thread/thread_local_storage.h"

namespace base {

namespace internal {

// static
bool PlatformThreadLocalStorage::AllocTLS(TLSKey* key) {
  return !pthread_key_create(key, nullptr);
}

// static
void PlatformThreadLocalStorage::FreeTLS(TLSKey key) {
  int ret = pthread_key_delete(key);
  DCHECK_EQ(ret, 0);
}

// static
void PlatformThreadLocalStorage::SetTLSValue(TLSKey key, void* value) {
  int ret = pthread_setspecific(key, value);
  DCHECK_EQ(ret, 0);
}

// static
void* PlatformThreadLocalStorage::GetTLSValue(TLSKey key) {
  return pthread_getspecific(key);
}

}  // namespace internal

}  // namespace base
