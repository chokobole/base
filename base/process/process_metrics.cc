// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/process/process_metrics.h"

#include <utility>

#include "absl/time/clock.h"
#include "base/build_config.h"
#include "base/logging.h"

namespace {
int CalculateEventsPerSecond(uint64_t event_count, uint64_t* last_event_count,
                             absl::Time* last_calculated) {
  absl::Time time = absl::Now();

  if (*last_event_count == 0) {
    // First call, just set the last values.
    *last_calculated = time;
    *last_event_count = event_count;
    return 0;
  }

  int64_t events_delta = event_count - *last_event_count;
  int64_t time_delta = absl::ToInt64Microseconds(time - *last_calculated);
  if (time_delta == 0) {
    NOTREACHED();
    return 0;
  }

  *last_calculated = time;
  *last_event_count = event_count;

  int64_t events_delta_for_ms = events_delta * std::micro{}.den;
  // Round the result up by adding 1/2 (the second term resolves to 1/2 without
  // dropping down into floating point).
  return (events_delta_for_ms + time_delta / 2) / time_delta;
}

}  // namespace

namespace base {

SystemMemoryInfoKB::SystemMemoryInfoKB() = default;

SystemMemoryInfoKB::SystemMemoryInfoKB(const SystemMemoryInfoKB& other) =
    default;

SystemMetrics::SystemMetrics() { committed_memory_ = 0; }

SystemMetrics SystemMetrics::Sample() {
  SystemMetrics system_metrics;

  system_metrics.committed_memory_ = GetSystemCommitCharge();
#if defined(OS_LINUX) || defined(OS_ANDROID)
  GetSystemMemoryInfo(&system_metrics.memory_info_);
  GetVmStatInfo(&system_metrics.vmstat_info_);
  GetSystemDiskInfo(&system_metrics.disk_info_);
#endif
#if defined(OS_CHROMEOS)
  GetSwapInfo(&system_metrics.swap_info_);
#endif
#if defined(OS_WIN)
  GetSystemPerformanceInfo(&system_metrics.performance_);
#endif
  return system_metrics;
}

std::unique_ptr<ProcessMetrics> ProcessMetrics::CreateCurrentProcessMetrics() {
#if !defined(OS_MACOSX) || defined(OS_IOS)
  return CreateProcessMetrics(base::GetCurrentProcessHandle());
#else
  return CreateProcessMetrics(base::GetCurrentProcessHandle(), nullptr);
#endif  // !defined(OS_MACOSX) || defined(OS_IOS)
}

#if !defined(OS_FREEBSD) || !defined(OS_POSIX)
double ProcessMetrics::GetPlatformIndependentCPUUsage() {
  absl::Duration cumulative_cpu = GetCumulativeCPUUsage();
  absl::Time time = absl::Now();

  if (last_cumulative_cpu_ == absl::ZeroDuration()) {
    // First call, just set the last values.
    last_cumulative_cpu_ = cumulative_cpu;
    last_cpu_time_ = time;
    return 0;
  }

  absl::Duration system_time_delta = cumulative_cpu - last_cumulative_cpu_;
  absl::Duration time_delta = time - last_cpu_time_;
  DCHECK_NE(time_delta, absl::ZeroDuration());
  if (time_delta == absl::ZeroDuration()) return 0;

  last_cumulative_cpu_ = cumulative_cpu;
  last_cpu_time_ = time;

  return 100.0 * absl::ToDoubleMicroseconds(system_time_delta) /
         absl::ToDoubleMicroseconds(time_delta);
}
#endif

#if defined(OS_MACOSX) || defined(OS_LINUX) || defined(OS_AIX)
int ProcessMetrics::CalculateIdleWakeupsPerSecond(
    uint64_t absolute_idle_wakeups) {
  return CalculateEventsPerSecond(absolute_idle_wakeups,
                                  &last_absolute_idle_wakeups_,
                                  &last_idle_wakeups_time_);
}
#else
int ProcessMetrics::GetIdleWakeupsPerSecond() {
  NOTIMPLEMENTED();  // http://crbug.com/120488
  return 0;
}
#endif  // defined(OS_MACOSX) || defined(OS_LINUX) || defined(OS_AIX)

#if defined(OS_MACOSX)
int ProcessMetrics::CalculatePackageIdleWakeupsPerSecond(
    uint64_t absolute_package_idle_wakeups) {
  return CalculateEventsPerSecond(absolute_package_idle_wakeups,
                                  &last_absolute_package_idle_wakeups_,
                                  &last_package_idle_wakeups_time_);
}

#endif  // defined(OS_MACOSX)

#if !defined(OS_WIN)
uint64_t ProcessMetrics::GetCumulativeDiskUsageInBytes() {
  // Not implemented.
  return 0;
}
#endif

uint64_t ProcessMetrics::GetDiskUsageBytesPerSecond() {
  uint64_t cumulative_disk_usage = GetCumulativeDiskUsageInBytes();
  return CalculateEventsPerSecond(cumulative_disk_usage,
                                  &last_cumulative_disk_usage_,
                                  &last_disk_usage_time_);
}

}  // namespace base
