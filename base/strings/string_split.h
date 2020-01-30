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

#include "absl/strings/string_view.h"
#include "base/export.h"

namespace base {

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