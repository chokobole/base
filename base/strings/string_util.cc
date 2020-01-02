// Copyright (c) 2019 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/strings/string_util.h"

#include "absl/strings/ascii.h"
#include "base/no_destructor.h"

namespace base {

bool StartsWith(absl::string_view text, absl::string_view expected) {
  if (text.size() < expected.size()) return false;

  absl::string_view target = text.substr(0, expected.size());
  return target == expected;
}

bool EndsWith(absl::string_view text, absl::string_view expected) {
  if (text.size() < expected.size()) return false;

  absl::string_view target =
      text.substr(text.size() - expected.size(), expected.size());
  return target == expected;
}

bool ConsumePrefix(absl::string_view* text, absl::string_view expected) {
  if (StartsWith(*text, expected)) {
    text->remove_prefix(expected.size());
    return true;
  }
  return false;
}

bool ConsumeSuffix(absl::string_view* text, absl::string_view expected) {
  if (EndsWith(*text, expected)) {
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
  static const base::NoDestructor<std::string> s;
  return *s;
}

}  // namespace base