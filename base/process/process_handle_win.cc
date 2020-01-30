// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/process/process_handle.h"

#include <windows.h>
#include <tlhelp32.h>

#include "base/win/scoped_handle.h"

namespace base {

ProcessId GetCurrentProcId() { return ::GetCurrentProcessId(); }

ProcessHandle GetCurrentProcessHandle() { return ::GetCurrentProcess(); }

ProcessId GetProcId(ProcessHandle process) {
  if (process == base::kNullProcessHandle) return 0;
  // This returns 0 if we have insufficient rights to query the process handle.
  // Invalid handles or non-process handles will cause a hard failure.
  ProcessId result = GetProcessId(process);
  CHECK(result != 0 || GetLastError() != ERROR_INVALID_HANDLE)
      << "process handle = " << process;
  return result;
}

ProcessId GetParentProcessId(ProcessHandle process) {
  ProcessId child_pid = GetProcId(process);
  PROCESSENTRY32 process_entry;
  process_entry.dwSize = sizeof(PROCESSENTRY32);

  win::ScopedHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
  if (snapshot.IsValid() && Process32First(snapshot.Get(), &process_entry)) {
    do {
      if (process_entry.th32ProcessID == child_pid)
        return process_entry.th32ParentProcessID;
    } while (Process32Next(snapshot.Get(), &process_entry));
  }

  return -1;
}

}  // namespace base
