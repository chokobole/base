// Copyright (c) 2019 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/strings/string_util.h"

#include "gtest/gtest.h"

namespace base {

TEST(StringsUtil, StartsWith) {
  absl::string_view sv = "Hello World";
  EXPECT_FALSE(StartsWith(sv, "World"));
  EXPECT_TRUE(StartsWith(sv, "Hello"));
  EXPECT_FALSE(StartsWith(sv, "Hello World!"));
}

TEST(StringsUtil, EndsWith) {
  absl::string_view sv = "Hello World";
  EXPECT_FALSE(EndsWith(sv, "Hello"));
  EXPECT_TRUE(EndsWith(sv, "World"));
  EXPECT_FALSE(EndsWith(sv, "!Hello World"));
}

}  // namespace base