// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/process/environment_internal.h"

#include <memory>
#include <vector>

#include "base/build_config.h"
#include "gtest/gtest.h"

namespace base {
namespace internal {

#if defined(OS_WIN)

namespace {
void ExpectEnvironmentBlock(const std::vector<std::string>& vars,
                            const std::string& block) {
  std::string expected;
  for (const auto& var : vars) {
    expected += var;
    expected.push_back('\0');
  }
  expected.push_back('\0');
  EXPECT_EQ(expected, block);
}
}  // namespace

TEST(EnvironmentInternalTest, AlterEnvironment) {
  const char empty[] = {'\0'};
  const char a2[] = {'A', '=', '2', '\0', '\0'};
  const char a2b3[] = {'A', '=', '2', '\0', 'B', '=', '3', '\0', '\0'};
  EnvironmentMap changes;
  std::string e;

  e = AlterEnvironment(empty, changes);
  ExpectEnvironmentBlock({}, e);

  changes["A"] = "1";
  e = AlterEnvironment(empty, changes);
  ExpectEnvironmentBlock({"A=1"}, e);

  changes.clear();
  changes["A"] = std::string();
  e = AlterEnvironment(empty, changes);
  ExpectEnvironmentBlock({}, e);

  changes.clear();
  e = AlterEnvironment(a2, changes);
  ExpectEnvironmentBlock({"A=2"}, e);

  changes.clear();
  changes["A"] = "1";
  e = AlterEnvironment(a2, changes);
  ExpectEnvironmentBlock({"A=1"}, e);

  changes.clear();
  changes["A"] = std::string();
  e = AlterEnvironment(a2, changes);
  ExpectEnvironmentBlock({}, e);

  changes.clear();
  changes["A"] = std::string();
  changes["B"] = std::string();
  e = AlterEnvironment(a2b3, changes);
  ExpectEnvironmentBlock({}, e);

  changes.clear();
  changes["A"] = std::string();
  e = AlterEnvironment(a2b3, changes);
  ExpectEnvironmentBlock({"B=3"}, e);

  changes.clear();
  changes["B"] = std::string();
  e = AlterEnvironment(a2b3, changes);
  ExpectEnvironmentBlock({"A=2"}, e);

  changes.clear();
  changes["A"] = "1";
  changes["C"] = "4";
  e = AlterEnvironment(a2b3, changes);
  // AlterEnvironment() currently always puts changed entries at the end.
  ExpectEnvironmentBlock({"B=3", "A=1", "C=4"}, e);
}

#else  // !OS_WIN

TEST(EnvironmentInternalTest, AlterEnvironment) {
  const char* const empty[] = {nullptr};
  const char* const a2[] = {"A=2", nullptr};
  const char* const a2b3[] = {"A=2", "B=3", nullptr};
  EnvironmentMap changes;
  std::unique_ptr<char*[]> e;

  e = AlterEnvironment(empty, changes);
  EXPECT_TRUE(e[0] == nullptr);

  changes["A"] = "1";
  e = AlterEnvironment(empty, changes);
  EXPECT_EQ(std::string("A=1"), e[0]);
  EXPECT_TRUE(e[1] == nullptr);

  changes.clear();
  changes["A"] = std::string();
  e = AlterEnvironment(empty, changes);
  EXPECT_TRUE(e[0] == nullptr);

  changes.clear();
  e = AlterEnvironment(a2, changes);
  EXPECT_EQ(std::string("A=2"), e[0]);
  EXPECT_TRUE(e[1] == nullptr);

  changes.clear();
  changes["A"] = "1";
  e = AlterEnvironment(a2, changes);
  EXPECT_EQ(std::string("A=1"), e[0]);
  EXPECT_TRUE(e[1] == nullptr);

  changes.clear();
  changes["A"] = std::string();
  e = AlterEnvironment(a2, changes);
  EXPECT_TRUE(e[0] == nullptr);

  changes.clear();
  changes["A"] = std::string();
  changes["B"] = std::string();
  e = AlterEnvironment(a2b3, changes);
  EXPECT_TRUE(e[0] == nullptr);

  changes.clear();
  changes["A"] = std::string();
  e = AlterEnvironment(a2b3, changes);
  EXPECT_EQ(std::string("B=3"), e[0]);
  EXPECT_TRUE(e[1] == nullptr);

  changes.clear();
  changes["B"] = std::string();
  e = AlterEnvironment(a2b3, changes);
  EXPECT_EQ(std::string("A=2"), e[0]);
  EXPECT_TRUE(e[1] == nullptr);

  changes.clear();
  changes["A"] = "1";
  changes["C"] = "4";
  e = AlterEnvironment(a2b3, changes);
  EXPECT_EQ(std::string("B=3"), e[0]);
  // AlterEnvironment() currently always puts changed entries at the end.
  EXPECT_EQ(std::string("A=1"), e[1]);
  EXPECT_EQ(std::string("C=4"), e[2]);
  EXPECT_TRUE(e[3] == nullptr);
}

#endif  // OS_WIN

}  // namespace internal
}  // namespace base
