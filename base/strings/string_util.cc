// Copyright (c) 2019 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/strings/string_util.h"

#include <algorithm>

#include "absl/strings/ascii.h"
#include "base/no_destructor.h"

namespace base {

namespace {

struct CaseInsensitiveCompareASCII {
 public:
  bool operator()(char x, char y) const {
    return absl::ascii_toupper(x) == absl::ascii_toupper(y);
  }
};

}  // namespace

bool IsStringASCII(absl::string_view text) {
  return std::all_of(text.begin(), text.end(), absl::ascii_isascii);
}

bool StartsWith(absl::string_view text, absl::string_view expected ,CompareCase compare_case) {
  if (text.size() < expected.size()) return false;

  absl::string_view target = text.substr(0, expected.size());
  if (compare_case == CompareCase::SENSITIVE_ASCII) {
    return target == expected;
  } else {
    return std::equal(
          expected.begin(), expected.end(),
          text.begin(),
          CaseInsensitiveCompareASCII());
  }
}

bool EndsWith(absl::string_view text, absl::string_view expected, CompareCase compare_case) {
  if (text.size() < expected.size()) return false;

  absl::string_view target =
      text.substr(text.size() - expected.size(), expected.size());
  if (compare_case == CompareCase::SENSITIVE_ASCII) {
    return target == expected;
  } else {
    return std::equal(
          expected.begin(), expected.end(),
          text.begin(),
          CaseInsensitiveCompareASCII());
  }
}

bool ConsumePrefix(absl::string_view* text, absl::string_view expected, CompareCase compare_case) {
  if (StartsWith(*text, expected, compare_case)) {
    text->remove_prefix(expected.size());
    return true;
  }
  return false;
}

bool ConsumeSuffix(absl::string_view* text, absl::string_view expected, CompareCase compare_case) {
  if (EndsWith(*text, expected, compare_case)) {
    text->remove_suffix(expected.size());
    return true;
  }
  return false;
}

bool ConsumeASCIIWhitespace(absl::string_view* text,
                            absl::string_view expected) {
  size_t whitespaces = 0;
  while (whitespaces < text->size()) {
    if (absl::ascii_isascii((*text)[whitespaces])) {
      whitespaces++;
    } else {
      break;
    }
  }
  if (whitespaces > 0) {
    text->remove_prefix(whitespaces);
    return true;
  }
  return false;
}

const std::string& EmptyString() {
  static const NoDestructor<std::string> s;
  return *s;
}

}  // namespace base