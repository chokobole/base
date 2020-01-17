// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <windows.h>

#include "base/logging.h"
#include "base/thread/thread_local_storage.h"

namespace base {

namespace internal {

// static
bool PlatformThreadLocalStorage::AllocTLS(TLSKey* key) {
  TLSKey value = TlsAlloc();
  if (value != TLS_OUT_OF_INDEXES) {
    *key = value;
    return true;
  }
  return false;
}

// static
void PlatformThreadLocalStorage::FreeTLS(TLSKey key) {
  BOOL ret = TlsFree(key);
  DCHECK(ret);
}

// static
void PlatformThreadLocalStorage::SetTLSValue(TLSKey key, void* value) {
  BOOL ret = TlsSetValue(key, value);
  DCHECK(ret);
}

// static
void* PlatformThreadLocalStorage::GetTLSValue(TLSKey key) {
  return TlsGetValue(key);
}

}  // namespace internal

}  // namespace base
