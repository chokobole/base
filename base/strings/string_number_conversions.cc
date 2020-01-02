// Copyright (c) 2019 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/strings/string_number_conversions.h"

namespace base {

bool StringToInt(absl::string_view input, int* output) {
  return absl::SimpleAtoi(input, output);
}

bool StringToUint(absl::string_view input, unsigned* output) {
  return absl::SimpleAtoi(input, output);
}

bool StringToInt64(absl::string_view input, int64_t* output) {
  return absl::SimpleAtoi(input, output);
}

bool StringToUint64(absl::string_view input, uint64_t* output) {
  return absl::SimpleAtoi(input, output);
}

bool StringToFloat(absl::string_view input, float* output) {
  return absl::SimpleAtof(input, output);
}

bool StringToDouble(absl::string_view input, double* output) {
  return absl::SimpleAtod(input, output);
}

bool HexStringToInt(absl::string_view input, int* output) {
  return absl::numbers_internal::safe_strtoi_base(input, output, 16);
}

bool HexStringToUint(absl::string_view input, unsigned* output) {
  return absl::numbers_internal::safe_strtoi_base(input, output, 16);
}

bool HexStringToInt64(absl::string_view input, int64_t* output) {
  return absl::numbers_internal::safe_strtoi_base(input, output, 16);
}

bool HexStringToUint64(absl::string_view input, uint64_t* output) {
  return absl::numbers_internal::safe_strtoi_base(input, output, 16);
}

}  // namespace base