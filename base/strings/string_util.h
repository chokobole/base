// Copyright (c) 2019 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STRINGS_STRING_H_
#define BASE_STRINGS_STRING_H_

#include <vector>

#include "absl/strings/string_view.h"
#include "base/export.h"

namespace base {

// Contains the set of characters representing whitespace in the corresponding
// encoding. Null-terminated. The ASCII versions are the whitespaces as defined
// by HTML5, and don't include control characters.
BASE_EXPORT extern const wchar_t kWhitespaceWide[];  // Includes Unicode.
BASE_EXPORT extern const char kWhitespaceASCII[];

BASE_EXPORT bool IsStringASCII(absl::string_view text);

enum class CompareCase {
  SENSITIVE_ASCII,
  INSENSITIVE_ASCII,
};

BASE_EXPORT bool StartsWith(
    absl::string_view text, absl::string_view expected,
    CompareCase compare_case = CompareCase::SENSITIVE_ASCII);

BASE_EXPORT bool EndsWith(
    absl::string_view text, absl::string_view expected,
    CompareCase compare_case = CompareCase::SENSITIVE_ASCII);

BASE_EXPORT bool ConsumePrefix(
    absl::string_view* text, absl::string_view expected,
    CompareCase compare_case = CompareCase::SENSITIVE_ASCII);

BASE_EXPORT bool ConsumeSuffix(
    absl::string_view* text, absl::string_view expected,
    CompareCase compare_case = CompareCase::SENSITIVE_ASCII);

BASE_EXPORT bool ConsumeASCIIWhitespace(absl::string_view* text);

BASE_EXPORT const std::string& EmptyString();

}  // namespace base

#endif  // BASE_STRINGS_STRING_H_