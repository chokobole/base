// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/system/sys_info.h"

#include <stddef.h>
#include <stdint.h>
#include <windows.h>

#include <limits>

#include "absl/strings/str_format.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/process/process_metrics.h"
#include "base/strings/string_util.h"
#include "base/win/windows_version.h"
#include "base/win/wmi.h"

namespace {

int64_t AmountOfMemory(DWORDLONG MEMORYSTATUSEX::*memory_field) {
  MEMORYSTATUSEX memory_info;
  memory_info.dwLength = sizeof(memory_info);
  if (!GlobalMemoryStatusEx(&memory_info)) {
    NOTREACHED();
    return 0;
  }

  int64_t rv = static_cast<int64_t>(memory_info.*memory_field);
  return rv < 0 ? std::numeric_limits<int64_t>::max() : rv;
}

bool GetDiskSpaceInfo(const base::FilePath& path, int64_t* available_bytes,
                      int64_t* total_bytes) {
  ULARGE_INTEGER available;
  ULARGE_INTEGER total;
  ULARGE_INTEGER free;
  if (!GetDiskFreeSpaceExA(path.value().c_str(), &available, &total, &free))
    return false;

  if (available_bytes) {
    *available_bytes = static_cast<int64_t>(available.QuadPart);
    if (*available_bytes < 0)
      *available_bytes = std::numeric_limits<int64_t>::max();
  }
  if (total_bytes) {
    *total_bytes = static_cast<int64_t>(total.QuadPart);
    if (*total_bytes < 0) *total_bytes = std::numeric_limits<int64_t>::max();
  }
  return true;
}

}  // namespace

namespace base {

// static
int SysInfo::NumberOfProcessors() {
  // return win::OSInfo::GetInstance()->processors();
  return 1;
}

// static
int64_t SysInfo::AmountOfPhysicalMemoryImpl() {
  return AmountOfMemory(&MEMORYSTATUSEX::ullTotalPhys);
}

// static
int64_t SysInfo::AmountOfAvailablePhysicalMemoryImpl() {
  SystemMemoryInfoKB info;
  if (!GetSystemMemoryInfo(&info)) return 0;
  return static_cast<int64_t>(info.avail_phys) * 1024;
}

// static
int64_t SysInfo::AmountOfVirtualMemory() {
  return AmountOfMemory(&MEMORYSTATUSEX::ullTotalVirtual);
}

// static
int64_t SysInfo::AmountOfFreeDiskSpace(const FilePath& path) {
  int64_t available;
  if (!GetDiskSpaceInfo(path, &available, nullptr)) return -1;
  return available;
}

// static
int64_t SysInfo::AmountOfTotalDiskSpace(const FilePath& path) {
  int64_t total;
  if (!GetDiskSpaceInfo(path, nullptr, &total)) return -1;
  return total;
}

std::string SysInfo::OperatingSystemName() { return "Windows NT"; }

// static
std::string SysInfo::OperatingSystemVersion() {
  // win::OSInfo* os_info = win::OSInfo::GetInstance();
  // win::OSInfo::VersionNumber version_number = os_info->version_number();
  // std::string version(absl::StrFormat("%d.%d.%d", version_number.major,
  //                                     version_number.minor,
  //                                     version_number.build));
  // win::OSInfo::ServicePack service_pack = os_info->service_pack();
  // if (service_pack.major != 0) {
  //   version += absl::StrFormat(" SP%d", service_pack.major);
  //   if (service_pack.minor != 0)
  //     version += absl::StrFormat(".%d", service_pack.minor);
  // }
  // return version;
  return "";
}

// TODO: Implement OperatingSystemVersionComplete, which would include
// patchlevel/service pack number.
// See chrome/browser/feedback/feedback_util.h, FeedbackUtil::SetOSVersion.

// static
std::string SysInfo::OperatingSystemArchitecture() {
  // win::OSInfo::WindowsArchitecture arch = win::OSInfo::GetArchitecture();
  // switch (arch) {
  //   case win::OSInfo::X86_ARCHITECTURE:
  //     return "x86";
  //   case win::OSInfo::X64_ARCHITECTURE:
  //     return "x86_64";
  //   case win::OSInfo::IA64_ARCHITECTURE:
  //     return "ia64";
  //   case win::OSInfo::ARM64_ARCHITECTURE:
  //     return "arm64";
  //   default:
  //     return "";
  // }
  return "";
}

// static
size_t SysInfo::VMAllocationGranularity() {
  // return win::OSInfo::GetInstance()->allocation_granularity();
  return 0;
}

// static
void SysInfo::OperatingSystemVersionNumbers(int32_t* major_version,
                                            int32_t* minor_version,
                                            int32_t* bugfix_version) {
  // win::OSInfo* os_info = win::OSInfo::GetInstance();
  // *major_version = os_info->version_number().major;
  // *minor_version = os_info->version_number().minor;
  // *bugfix_version = 0;
}

// static
SysInfo::HardwareInfo SysInfo::GetHardwareInfoSync() {
  // win::WmiComputerSystemInfo wmi_info = win::WmiComputerSystemInfo::Get();

  HardwareInfo info;
  // info.manufacturer = wmi_info.manufacturer();
  // info.model = wmi_info.model();
  // info.serial_number = wmi_info.serial_number();
  return info;
}

}  // namespace base
