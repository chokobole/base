// Copyright (c) 2019 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STRINGS_STRING_NUMBER_CONVERSIONS_H_
#define BASE_STRINGS_STRING_NUMBER_CONVERSIONS_H_

#include <string>
#include <type_traits>

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "base/export.h"

namespace base {

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value>* = nullptr>
std::string NumberToString(T value) {
  return absl::StrCat(value);
}

BASE_EXPORT bool StringToInt(absl::string_view input, int* output);
BASE_EXPORT bool StringToUint(absl::string_view input, unsigned* output);
BASE_EXPORT bool StringToInt64(absl::string_view input, int64_t* output);
BASE_EXPORT bool StringToUint64(absl::string_view input, uint64_t* output);
BASE_EXPORT bool StringToFloat(absl::string_view input, float* output);
BASE_EXPORT bool StringToDouble(absl::string_view input, double* output);

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value>* = nullptr>
std::string HexToString(T value) {
  return absl::StrCat(absl::Hex(value));
}

BASE_EXPORT bool HexStringToInt(absl::string_view input, int* output);
BASE_EXPORT bool HexStringToUint(absl::string_view input, unsigned* output);
BASE_EXPORT bool HexStringToInt64(absl::string_view input, int64_t* output);
BASE_EXPORT bool HexStringToUint64(absl::string_view input, uint64_t* output);

}  // namespace base

#endif  // BASE_STRINGS_STRING_NUMBER_CONVERSIONS_H_