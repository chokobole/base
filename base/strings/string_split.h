// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Followings are taken and modified form base/strings/string_split.h
// bool SplitStringIntoKeyValuePairs(absl::string_view input,
//                                   char key_value_delimiter,
//                                   char key_value_pair_delimiter,
//                                   StringPairs* key_value_pairs)

#include <string>
#include <utility>
#include <vector>

#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "base/export.h"

namespace base {

enum WhitespaceHandling {
  KEEP_WHITESPACE,
  TRIM_WHITESPACE,
};

enum SplitResult {
  // Strictly return all results.
  //
  // If the input is ",," and the separator is ',' this will return a
  // vector of three empty strings.
  SPLIT_WANT_ALL,

  // Only nonempty results will be added to the results. Multiple separators
  // will be coalesced. Separators at the beginning and end of the input will
  // be ignored. With TRIM_WHITESPACE, whitespace-only results will be dropped.
  //
  // If the input is ",," and the separator is ',', this will return an empty
  // vector.
  SPLIT_WANT_NONEMPTY,
};

BASE_EXPORT std::vector<std::string> SplitString(absl::string_view input,
                                                 absl::string_view separators,
                                                 WhitespaceHandling whitespace,
                                                 SplitResult result_type);

BASE_EXPORT std::vector<absl::string_view> SplitStringView(
    absl::string_view input, absl::string_view separators,
    WhitespaceHandling whitespace, SplitResult result_type);

using StringPair = std::pair<std::string, std::string>;
using StringPairs = std::vector<StringPair>;

// Splits |line| into key value pairs according to the given delimiters and
// removes whitespace leading each key and trailing each value. Returns true
// only if each pair has a non-empty key and value. |key_value_pairs| will
// include ("","") pairs for entries without |key_value_delimiter|.
BASE_EXPORT bool SplitStringIntoKeyValuePairs(absl::string_view input,
                                              char key_value_delimiter,
                                              char key_value_pair_delimiter,
                                              StringPairs* key_value_pairs);

}  // namespace base