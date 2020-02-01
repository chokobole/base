// Copyright (c) 2019 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STRINGS_STRING_H_
#define BASE_STRINGS_STRING_H_

#include "absl/strings/string_view.h"
#include "base/export.h"

namespace base {

bool IsStringASCII(absl::string_view text);

enum class CompareCase {
  SENSITIVE_ASCII,
  INSENSITIVE_ASCII,
};

BASE_EXPORT bool StartsWith(absl::string_view text, absl::string_view expected, CompareCase compare_case = CompareCase::SENSITIVE_ASCII);

BASE_EXPORT bool EndsWith(absl::string_view text, absl::string_view expected, CompareCase compare_case = CompareCase::SENSITIVE_ASCII);

BASE_EXPORT bool ConsumePrefix(absl::string_view* text,
                               absl::string_view expected,
                               CompareCase compare_case = CompareCase::SENSITIVE_ASCII);

BASE_EXPORT bool ConsumeSuffix(absl::string_view* text,
                               absl::string_view expected,
                               CompareCase compare_case = CompareCase::SENSITIVE_ASCII);

BASE_EXPORT bool ConsumeASCIIWhitespace(absl::string_view* text,
                                        absl::string_view expected);

BASE_EXPORT const std::string& EmptyString();

}  // namespace base

#endif  // BASE_STRINGS_STRING_H_