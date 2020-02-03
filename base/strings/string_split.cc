// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/strings/string_split.h"

#include "absl/strings/str_split.h"
#include "base/logging.h"

namespace base {

namespace {

void TrimWhitespace(std::vector<std::string>* texts) {
  std::for_each(texts->begin(), texts->end(),
                [](std::string& t) { absl::StripAsciiWhitespace(&t); });
}

void TrimWhitespace(std::vector<absl::string_view>* texts) {
  std::for_each(texts->begin(), texts->end(), [](absl::string_view& sv) {
    sv = absl::StripAsciiWhitespace(sv);
  });
}

}  // namespace

std::vector<std::string> SplitString(absl::string_view input,
                                     absl::string_view separators,
                                     WhitespaceHandling whitespace,
                                     SplitResult result_type) {
  std::vector<std::string> result;
  if (result_type == SplitResult::SPLIT_WANT_NONEMPTY) {
    result = absl::StrSplit(input, separators, absl::SkipEmpty());
  } else {
    result = absl::StrSplit(input, separators);
  }
  if (whitespace == WhitespaceHandling::TRIM_WHITESPACE) {
    TrimWhitespace(&result);
  }
  return result;
}

std::vector<absl::string_view> SplitStringView(absl::string_view input,
                                               absl::string_view separators,
                                               WhitespaceHandling whitespace,
                                               SplitResult result_type) {
  std::vector<absl::string_view> result;
  if (result_type == SplitResult::SPLIT_WANT_NONEMPTY) {
    result = absl::StrSplit(input, separators, absl::SkipEmpty());
  } else {
    result = absl::StrSplit(input, separators);
  }
  if (whitespace == WhitespaceHandling::TRIM_WHITESPACE) {
    TrimWhitespace(&result);
  }
  return result;
}

bool SplitStringIntoKeyValuePairs(absl::string_view input,
                                  char key_value_delimiter,
                                  char key_value_pair_delimiter,
                                  StringPairs* key_value_pairs) {
  std::vector<absl::string_view> pairs =
      absl::StrSplit(input, key_value_pair_delimiter);
  key_value_pairs->reserve(pairs.size());

  for (absl::string_view pair : pairs) {
    size_t end_key_pos = pair.find_first_of(key_value_delimiter);
    if (end_key_pos == std::string::npos) {
      DVLOG(1) << "cannot find delimiter in: " << pair;
      return false;  // No delimiter.
    }

    StringPair string_pair;
    string_pair.first = std::string(pair.substr(0, end_key_pos));
    string_pair.second =
        std::string(pair.substr(end_key_pos, pair.size() - end_key_pos));
    key_value_pairs->push_back(std::move(string_pair));
  }

  return key_value_pairs->size() > 0;
}

}  // namespace base