// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/file_path.h"

#include <stddef.h>

#include <sstream>

#include "base/stl_util.h"
#include "gtest/gtest.h"

namespace base {

struct UnaryTestData {
  absl::string_view input;
  absl::string_view expected;
};

struct UnaryBooleanTestData {
  absl::string_view input;
  bool expected;
};

struct BinaryTestData {
  absl::string_view inputs[2];
  absl::string_view expected;
};

struct BinaryBooleanTestData {
  absl::string_view inputs[2];
  bool expected;
};

struct BinaryIntTestData {
  absl::string_view inputs[2];
  int expected;
};

struct UTF8TestData {
  absl::string_view native;
  absl::string_view utf8;
};

TEST(FilePathTest, DirName) {
  const struct UnaryTestData cases[] = {
    {"", "."},
    {"aa", "."},
    {"/aa/bb", "/aa"},
    {"/aa/bb/", "/aa"},
    {"/aa/bb//", "/aa"},
    {"/aa/bb/ccc", "/aa/bb"},
    {"/aa", "/"},
    {"/aa/", "/"},
    {"/", "/"},
    {"//", "//"},
    {"///", "/"},
    {"aa/", "."},
    {"aa/bb", "aa"},
    {"aa/bb/", "aa"},
    {"aa/bb//", "aa"},
    {"aa//bb//", "aa"},
    {"aa//bb/", "aa"},
    {"aa//bb", "aa"},
    {"//aa/bb", "//aa"},
    {"//aa/", "//"},
    {"//aa", "//"},
    {"0:", "."},
    {"@:", "."},
    {"[:", "."},
    {"`:", "."},
    {"{:", "."},
    {"\xB3:", "."},
    {"\xC5:", "."},
    {"/aa/../bb/cc", "/aa/../bb"},
#if defined(OS_WIN)
  // {"\x0143:", "."},
#endif  // OS_WIN
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {"c:", "c:"},
    {"C:", "C:"},
    {"A:", "A:"},
    {"Z:", "Z:"},
    {"a:", "a:"},
    {"z:", "z:"},
    {"c:aa", "c:"},
    {"c:/", "c:/"},
    {"c://", "c://"},
    {"c:///", "c:/"},
    {"c:/aa", "c:/"},
    {"c:/aa/", "c:/"},
    {"c:/aa/bb", "c:/aa"},
    {"c:aa/bb", "c:aa"},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
    {"\\aa\\bb", "\\aa"},
    {"\\aa\\bb\\", "\\aa"},
    {"\\aa\\bb\\\\", "\\aa"},
    {"\\aa\\bb\\ccc", "\\aa\\bb"},
    {"\\aa", "\\"},
    {"\\aa\\", "\\"},
    {"\\", "\\"},
    {"\\\\", "\\\\"},
    {"\\\\\\", "\\"},
    {"aa\\", "."},
    {"aa\\bb", "aa"},
    {"aa\\bb\\", "aa"},
    {"aa\\bb\\\\", "aa"},
    {"aa\\\\bb\\\\", "aa"},
    {"aa\\\\bb\\", "aa"},
    {"aa\\\\bb", "aa"},
    {"\\\\aa\\bb", "\\\\aa"},
    {"\\\\aa\\", "\\\\"},
    {"\\\\aa", "\\\\"},
    {"aa\\..\\bb\\c", "aa\\..\\bb"},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {"c:\\", "c:\\"},
    {"c:\\\\", "c:\\\\"},
    {"c:\\\\\\", "c:\\"},
    {"c:\\aa", "c:\\"},
    {"c:\\aa\\", "c:\\"},
    {"c:\\aa\\bb", "c:\\aa"},
    {"c:aa\\bb", "c:aa"},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  };

  for (size_t i = 0; i < size(cases); ++i) {
    FilePath input(cases[i].input);
    FilePath observed = input.DirName();
    EXPECT_EQ(std::string(cases[i].expected), observed.value())
        << "i: " << i << ", input: " << input.value();
  }
}

TEST(FilePathTest, BaseName) {
  const struct UnaryTestData cases[] = {
    {"", ""},
    {"aa", "aa"},
    {"/aa/bb", "bb"},
    {"/aa/bb/", "bb"},
    {"/aa/bb//", "bb"},
    {"/aa/bb/ccc", "ccc"},
    {"/aa", "aa"},
    {"/", "/"},
    {"//", "//"},
    {"///", "/"},
    {"aa/", "aa"},
    {"aa/bb", "bb"},
    {"aa/bb/", "bb"},
    {"aa/bb//", "bb"},
    {"aa//bb//", "bb"},
    {"aa//bb/", "bb"},
    {"aa//bb", "bb"},
    {"//aa/bb", "bb"},
    {"//aa/", "aa"},
    {"//aa", "aa"},
    {"0:", "0:"},
    {"@:", "@:"},
    {"[:", "[:"},
    {"`:", "`:"},
    {"{:", "{:"},
    {"\xB3:", "\xB3:"},
    {"\xC5:", "\xC5:"},
#if defined(OS_WIN)
    // {"\x0143:", "\x0143:"},
#endif  // OS_WIN
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {"c:", ""},
    {"C:", ""},
    {"A:", ""},
    {"Z:", ""},
    {"a:", ""},
    {"z:", ""},
    {"c:aa", "aa"},
    {"c:/", "/"},
    {"c://", "//"},
    {"c:///", "/"},
    {"c:/aa", "aa"},
    {"c:/aa/", "aa"},
    {"c:/aa/bb", "bb"},
    {"c:aa/bb", "bb"},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
    {"\\aa\\bb", "bb"},
    {"\\aa\\bb\\", "bb"},
    {"\\aa\\bb\\\\", "bb"},
    {"\\aa\\bb\\ccc", "ccc"},
    {"\\aa", "aa"},
    {"\\", "\\"},
    {"\\\\", "\\\\"},
    {"\\\\\\", "\\"},
    {"aa\\", "aa"},
    {"aa\\bb", "bb"},
    {"aa\\bb\\", "bb"},
    {"aa\\bb\\\\", "bb"},
    {"aa\\\\bb\\\\", "bb"},
    {"aa\\\\bb\\", "bb"},
    {"aa\\\\bb", "bb"},
    {"\\\\aa\\bb", "bb"},
    {"\\\\aa\\", "aa"},
    {"\\\\aa", "aa"},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {"c:\\", "\\"},
    {"c:\\\\", "\\\\"},
    {"c:\\\\\\", "\\"},
    {"c:\\aa", "aa"},
    {"c:\\aa\\", "aa"},
    {"c:\\aa\\bb", "bb"},
    {"c:aa\\bb", "bb"},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  };

  for (size_t i = 0; i < size(cases); ++i) {
    FilePath input(cases[i].input);
    FilePath observed = input.BaseName();
    EXPECT_EQ(std::string(cases[i].expected), observed.value())
        << "i: " << i << ", input: " << input.value();
  }
}

TEST(FilePathTest, Append) {
  const struct BinaryTestData cases[] = {
    {{"", "cc"}, "cc"},
    {{".", "ff"}, "ff"},
    {{".", ""}, "."},
    {{"/", "cc"}, "/cc"},
    {{"/aa", ""}, "/aa"},
    {{"/aa/", ""}, "/aa"},
    {{"//aa", ""}, "//aa"},
    {{"//aa/", ""}, "//aa"},
    {{"//", "aa"}, "//aa"},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {{"c:", "a"}, "c:a"},
    {{"c:", ""}, "c:"},
    {{"c:/", "a"}, "c:/a"},
    {{"c://", "a"}, "c://a"},
    {{"c:///", "a"}, "c:/a"},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
    // Append introduces the default separator character, so these test cases
    // need to be defined with different expected results on platforms that use
    // different default separator characters.
    {{"\\", "cc"}, "\\cc"},
    {{"\\aa", ""}, "\\aa"},
    {{"\\aa\\", ""}, "\\aa"},
    {{"\\\\aa", ""}, "\\\\aa"},
    {{"\\\\aa\\", ""}, "\\\\aa"},
    {{"\\\\", "aa"}, "\\\\aa"},
    {{"/aa/bb", "cc"}, "/aa/bb\\cc"},
    {{"/aa/bb/", "cc"}, "/aa/bb\\cc"},
    {{"aa/bb/", "cc"}, "aa/bb\\cc"},
    {{"aa/bb", "cc"}, "aa/bb\\cc"},
    {{"a/b", "c"}, "a/b\\c"},
    {{"a/b/", "c"}, "a/b\\c"},
    {{"//aa", "bb"}, "//aa\\bb"},
    {{"//aa/", "bb"}, "//aa\\bb"},
    {{"\\aa\\bb", "cc"}, "\\aa\\bb\\cc"},
    {{"\\aa\\bb\\", "cc"}, "\\aa\\bb\\cc"},
    {{"aa\\bb\\", "cc"}, "aa\\bb\\cc"},
    {{"aa\\bb", "cc"}, "aa\\bb\\cc"},
    {{"a\\b", "c"}, "a\\b\\c"},
    {{"a\\b\\", "c"}, "a\\b\\c"},
    {{"\\\\aa", "bb"}, "\\\\aa\\bb"},
    {{"\\\\aa\\", "bb"}, "\\\\aa\\bb"},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {{"c:\\", "a"}, "c:\\a"},
    {{"c:\\\\", "a"}, "c:\\\\a"},
    {{"c:\\\\\\", "a"}, "c:\\a"},
    {{"c:\\", ""}, "c:\\"},
    {{"c:\\a", "b"}, "c:\\a\\b"},
    {{"c:\\a\\", "b"}, "c:\\a\\b"},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#else   // FILE_PATH_USES_WIN_SEPARATORS
    {{"/aa/bb", "cc"}, "/aa/bb/cc"},
    {{"/aa/bb/", "cc"}, "/aa/bb/cc"},
    {{"aa/bb/", "cc"}, "aa/bb/cc"},
    {{"aa/bb", "cc"}, "aa/bb/cc"},
    {{"a/b", "c"}, "a/b/c"},
    {{"a/b/", "c"}, "a/b/c"},
    {{"//aa", "bb"}, "//aa/bb"},
    {{"//aa/", "bb"}, "//aa/bb"},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {{"c:/", "a"}, "c:/a"},
    {{"c:/", ""}, "c:/"},
    {{"c:/a", "b"}, "c:/a/b"},
    {{"c:/a/", "b"}, "c:/a/b"},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  };

  for (size_t i = 0; i < size(cases); ++i) {
    FilePath root(cases[i].inputs[0]);
    std::string leaf(cases[i].inputs[1]);
    FilePath observed_str = root.Append(leaf);
    EXPECT_EQ(std::string(cases[i].expected), observed_str.value())
        << "i: " << i << ", root: " << root.value() << ", leaf: " << leaf;
    FilePath observed_path = root.Append(FilePath(leaf));
    EXPECT_EQ(std::string(cases[i].expected), observed_path.value())
        << "i: " << i << ", root: " << root.value() << ", leaf: " << leaf;
    observed_str = root.Append(leaf);
    EXPECT_EQ(std::string(cases[i].expected), observed_str.value())
        << "i: " << i << ", root: " << root.value() << ", leaf: " << leaf;
  }
}

TEST(FilePathTest, StripTrailingSeparators) {
  const struct UnaryTestData cases[] = {
    {"", ""},
    {"/", "/"},
    {"//", "//"},
    {"///", "/"},
    {"////", "/"},
    {"a/", "a"},
    {"a//", "a"},
    {"a///", "a"},
    {"a////", "a"},
    {"/a", "/a"},
    {"/a/", "/a"},
    {"/a//", "/a"},
    {"/a///", "/a"},
    {"/a////", "/a"},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {"c:", "c:"},
    {"c:/", "c:/"},
    {"c://", "c://"},
    {"c:///", "c:/"},
    {"c:////", "c:/"},
    {"c:/a", "c:/a"},
    {"c:/a/", "c:/a"},
    {"c:/a//", "c:/a"},
    {"c:/a///", "c:/a"},
    {"c:/a////", "c:/a"},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
    {"\\", "\\"},
    {"\\\\", "\\\\"},
    {"\\\\\\", "\\"},
    {"\\\\\\\\", "\\"},
    {"a\\", "a"},
    {"a\\\\", "a"},
    {"a\\\\\\", "a"},
    {"a\\\\\\\\", "a"},
    {"\\a", "\\a"},
    {"\\a\\", "\\a"},
    {"\\a\\\\", "\\a"},
    {"\\a\\\\\\", "\\a"},
    {"\\a\\\\\\\\", "\\a"},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {"c:\\", "c:\\"},
    {"c:\\\\", "c:\\\\"},
    {"c:\\\\\\", "c:\\"},
    {"c:\\\\\\\\", "c:\\"},
    {"c:\\a", "c:\\a"},
    {"c:\\a\\", "c:\\a"},
    {"c:\\a\\\\", "c:\\a"},
    {"c:\\a\\\\\\", "c:\\a"},
    {"c:\\a\\\\\\\\", "c:\\a"},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  };

  for (size_t i = 0; i < size(cases); ++i) {
    FilePath input(cases[i].input);
    FilePath observed = input.StripTrailingSeparators();
    EXPECT_EQ(std::string(cases[i].expected), observed.value())
        << "i: " << i << ", input: " << input.value();
  }
}

TEST(FilePathTest, IsAbsolute) {
  const struct UnaryBooleanTestData cases[] = {
    {"", false},
    {"a", false},
    {"c:", false},
    {"c:a", false},
    {"a/b", false},
    {"//", true},
    {"//a", true},
    {"c:a/b", false},
    {"?:/a", false},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {"/", false},
    {"/a", false},
    {"/.", false},
    {"/..", false},
    {"c:/", true},
    {"c:/a", true},
    {"c:/.", true},
    {"c:/..", true},
    {"C:/a", true},
    {"d:/a", true},
#else   // FILE_PATH_USES_DRIVE_LETTERS
    {"/", true},
    {"/a", true},
    {"/.", true},
    {"/..", true},
    {"c:/", false},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
    {"a\\b", false},
    {"\\\\", true},
    {"\\\\a", true},
    {"a\\b", false},
    {"\\\\", true},
    {"//a", true},
    {"c:a\\b", false},
    {"?:\\a", false},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {"\\", false},
    {"\\a", false},
    {"\\.", false},
    {"\\..", false},
    {"c:\\", true},
    {"c:\\", true},
    {"c:\\a", true},
    {"c:\\.", true},
    {"c:\\..", true},
    {"C:\\a", true},
    {"d:\\a", true},
#else   // FILE_PATH_USES_DRIVE_LETTERS
    {"\\", true},
    {"\\a", true},
    {"\\.", true},
    {"\\..", true},
    {"c:\\", false},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  };

  for (size_t i = 0; i < size(cases); ++i) {
    FilePath input(cases[i].input);
    bool observed = input.IsAbsolute();
    EXPECT_EQ(cases[i].expected, observed)
        << "i: " << i << ", input: " << input.value();
  }
}

TEST(FilePathTest, PathComponentsTest) {
  const struct UnaryTestData cases[] = {
    {"//foo/bar/baz/", "|//|foo|bar|baz"},
    {"///", "|/"},
    {"/foo//bar//baz/", "|/|foo|bar|baz"},
    {"/foo/bar/baz/", "|/|foo|bar|baz"},
    {"/foo/bar/baz//", "|/|foo|bar|baz"},
    {"/foo/bar/baz///", "|/|foo|bar|baz"},
    {"/foo/bar/baz", "|/|foo|bar|baz"},
    {"/foo/bar.bot/baz.txt", "|/|foo|bar.bot|baz.txt"},
    {"//foo//bar/baz", "|//|foo|bar|baz"},
    {"/", "|/"},
    {"foo", "|foo"},
    {"", ""},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {"e:/foo", "|e:|/|foo"},
    {"e:/", "|e:|/"},
    {"e:", "|e:"},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
    {"../foo", "|..|foo"},
    {"./foo", "|foo"},
    {"../foo/bar/", "|..|foo|bar"},
    {"\\\\foo\\bar\\baz\\", "|\\\\|foo|bar|baz"},
    {"\\\\\\", "|\\"},
    {"\\foo\\\\bar\\\\baz\\", "|\\|foo|bar|baz"},
    {"\\foo\\bar\\baz\\", "|\\|foo|bar|baz"},
    {"\\foo\\bar\\baz\\\\", "|\\|foo|bar|baz"},
    {"\\foo\\bar\\baz\\\\\\", "|\\|foo|bar|baz"},
    {"\\foo\\bar\\baz", "|\\|foo|bar|baz"},
    {"\\foo\\bar/baz\\\\\\", "|\\|foo|bar|baz"},
    {"/foo\\bar\\baz", "|/|foo|bar|baz"},
    {"\\foo\\bar.bot\\baz.txt", "|\\|foo|bar.bot|baz.txt"},
    {"\\\\foo\\\\bar\\baz", "|\\\\|foo|bar|baz"},
    {"\\", "|\\"},
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  };

  for (size_t i = 0; i < size(cases); ++i) {
    FilePath input(cases[i].input);
    std::vector<std::string> comps;
    input.GetComponents(&comps);

    std::string observed;
    for (const auto& j : comps) {
      observed.append("|", 1);
      observed.append(j);
    }
    EXPECT_EQ(std::string(cases[i].expected), observed)
        << "i: " << i << ", input: " << input.value();
  }
}

TEST(FilePathTest, EqualityTest) {
  const struct BinaryBooleanTestData cases[] = {
    {{"/foo/bar/baz", "/foo/bar/baz"}, true},
    {{"/foo/bar", "/foo/bar/baz"}, false},
    {{"/foo/bar/baz", "/foo/bar"}, false},
    {{"//foo/bar/", "//foo/bar/"}, true},
    {{"/foo/bar", "/foo2/bar"}, false},
    {{"/foo/bar.txt", "/foo/bar"}, false},
    {{"foo/bar", "foo/bar"}, true},
    {{"foo/bar", "foo/bar/baz"}, false},
    {{"", "foo"}, false},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {{"c:/foo/bar", "c:/foo/bar"}, true},
    {{"E:/foo/bar", "e:/foo/bar"}, true},
    {{"f:/foo/bar", "F:/foo/bar"}, true},
    {{"E:/Foo/bar", "e:/foo/bar"}, false},
    {{"f:/foo/bar", "F:/foo/Bar"}, false},
    {{"c:/", "c:/"}, true},
    {{"c:", "c:"}, true},
    {{"c:/foo/bar", "d:/foo/bar"}, false},
    {{"c:/foo/bar", "D:/foo/bar"}, false},
    {{"C:/foo/bar", "d:/foo/bar"}, false},
    {{"c:/foo/bar", "c:/foo2/bar"}, false},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
    {{"\\foo\\bar", "\\foo\\bar"}, true},
    {{"\\foo/bar", "\\foo/bar"}, true},
    {{"\\foo/bar", "\\foo\\bar"}, false},
    {{"\\", "\\"}, true},
    {{"\\", "/"}, false},
    {{"", "\\"}, false},
    {{"\\foo\\bar", "\\foo2\\bar"}, false},
    {{"\\foo\\bar", "\\foo\\bar2"}, false},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
    {{"c:\\foo\\bar", "c:\\foo\\bar"}, true},
    {{"E:\\foo\\bar", "e:\\foo\\bar"}, true},
    {{"f:\\foo\\bar", "F:\\foo/bar"}, false},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  };

  for (size_t i = 0; i < size(cases); ++i) {
    FilePath a(cases[i].inputs[0]);
    FilePath b(cases[i].inputs[1]);

    EXPECT_EQ(a == b, cases[i].expected)
        << "equality i: " << i << ", a: " << a.value() << ", b: " << b.value();
  }

  for (size_t i = 0; i < size(cases); ++i) {
    FilePath a(cases[i].inputs[0]);
    FilePath b(cases[i].inputs[1]);

    EXPECT_EQ(a != b, !cases[i].expected)
        << "inequality i: " << i << ", a: " << a.value()
        << ", b: " << b.value();
  }
}

TEST(FilePathTest, Extension) {
  FilePath base_dir("base_dir");

  FilePath jpg = base_dir.Append("foo.jpg");
  EXPECT_EQ(".jpg", jpg.Extension());

  FilePath base = jpg.BaseName().RemoveExtension();
  EXPECT_EQ("foo", base.value());

  FilePath path_no_ext = base_dir.Append(base);
  EXPECT_EQ(path_no_ext.value(), jpg.RemoveExtension().value());

  EXPECT_EQ(path_no_ext.value(), path_no_ext.RemoveExtension().value());
  EXPECT_EQ("", path_no_ext.Extension());
}

TEST(FilePathTest, RemoveExtension) {
  const struct UnaryTestData cases[] = {
    {"", ""},
    {".", "."},
    {"..", ".."},
    {"foo.dll", "foo"},
    {"./foo.dll", "./foo"},
    {"foo..dll", "foo."},
    {"foo", "foo"},
    {"foo.", "foo"},
    {"foo..", "foo."},
    {"foo.baz.dll", "foo.baz"},
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
    {"C:\\foo.bar\\foo", "C:\\foo.bar\\foo"},
    {"C:\\foo.bar\\..\\\\", "C:\\foo.bar\\..\\\\"},
#endif
    {"/foo.bar/foo", "/foo.bar/foo"},
    {"/foo.bar/..////", "/foo.bar/..////"},
  };
  for (unsigned int i = 0; i < size(cases); ++i) {
    FilePath path(cases[i].input);
    FilePath removed = path.RemoveExtension();
    EXPECT_EQ(cases[i].expected, removed.value())
        << "i: " << i << ", path: " << path.value();
  }
  {
    FilePath path("foo.tar.gz");
    FilePath removed = path.RemoveExtension();
    EXPECT_EQ("foo.tar", removed.value()) << ", path: " << path.value();
  }
}

TEST(FilePathTest, ReplaceExtension) {
  const struct BinaryTestData cases[] = {
    {{"", ""}, ""},
    {{"", "txt"}, ""},
    {{".", "txt"}, ""},
    {{"..", "txt"}, ""},
    {{".", ""}, ""},
    {{"foo.dll", "txt"}, "foo.txt"},
    {{"./foo.dll", "txt"}, "./foo.txt"},
    {{"foo..dll", "txt"}, "foo..txt"},
    {{"foo.dll", ".txt"}, "foo.txt"},
    {{"foo", "txt"}, "foo.txt"},
    {{"foo.", "txt"}, "foo.txt"},
    {{"foo..", "txt"}, "foo..txt"},
    {{"foo", ".txt"}, "foo.txt"},
    {{"foo.baz.dll", "txt"}, "foo.baz.txt"},
    {{"foo.baz.dll", ".txt"}, "foo.baz.txt"},
    {{"foo.dll", ""}, "foo"},
    {{"foo.dll", "."}, "foo"},
    {{"foo", ""}, "foo"},
    {{"foo", "."}, "foo"},
    {{"foo.baz.dll", ""}, "foo.baz"},
    {{"foo.baz.dll", "."}, "foo.baz"},
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
    {{"C:\\foo.bar\\foo", "baz"}, "C:\\foo.bar\\foo.baz"},
    {{"C:\\foo.bar\\..\\\\", "baz"}, ""},
#endif
    {{"/foo.bar/foo", "baz"}, "/foo.bar/foo.baz"},
    {{"/foo.bar/..////", "baz"}, ""},
  };
  for (unsigned int i = 0; i < size(cases); ++i) {
    FilePath path(cases[i].inputs[0]);
    FilePath replaced = path.ReplaceExtension(cases[i].inputs[1]);
    EXPECT_EQ(cases[i].expected, replaced.value())
        << "i: " << i << ", path: " << path.value()
        << ", replace: " << cases[i].inputs[1];
  }
}

TEST(FilePathTest, AddExtension) {
  const struct BinaryTestData cases[] = {
    {{"", ""}, ""},
    {{"", "txt"}, ""},
    {{".", "txt"}, ""},
    {{"..", "txt"}, ""},
    {{".", ""}, ""},
    {{"foo.dll", "txt"}, "foo.dll.txt"},
    {{"./foo.dll", "txt"}, "./foo.dll.txt"},
    {{"foo..dll", "txt"}, "foo..dll.txt"},
    {{"foo.dll", ".txt"}, "foo.dll.txt"},
    {{"foo", "txt"}, "foo.txt"},
    {{"foo.", "txt"}, "foo.txt"},
    {{"foo..", "txt"}, "foo..txt"},
    {{"foo", ".txt"}, "foo.txt"},
    {{"foo.baz.dll", "txt"}, "foo.baz.dll.txt"},
    {{"foo.baz.dll", ".txt"}, "foo.baz.dll.txt"},
    {{"foo.dll", ""}, "foo.dll"},
    {{"foo.dll", "."}, "foo.dll"},
    {{"foo", ""}, "foo"},
    {{"foo", "."}, "foo"},
    {{"foo.baz.dll", ""}, "foo.baz.dll"},
    {{"foo.baz.dll", "."}, "foo.baz.dll"},
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
    {{"C:\\foo.bar\\foo", "baz"}, "C:\\foo.bar\\foo.baz"},
    {{"C:\\foo.bar\\..\\\\", "baz"}, ""},
#endif
    {{"/foo.bar/foo", "baz"}, "/foo.bar/foo.baz"},
    {{"/foo.bar/..////", "baz"}, ""},
  };
  for (unsigned int i = 0; i < size(cases); ++i) {
    FilePath path(cases[i].inputs[0]);
    FilePath added = path.AddExtension(cases[i].inputs[1]);
    EXPECT_EQ(cases[i].expected, added.value())
        << "i: " << i << ", path: " << path.value()
        << ", add: " << cases[i].inputs[1];
  }
}
TEST(FilePathTest, ConstructWithNUL) {
  // Test constructor strips '\0'
  FilePath path("a\0b");
  EXPECT_EQ(1U, path.value().length());
  EXPECT_EQ("a", path.value());
}

TEST(FilePathTest, EndsWithSeparator) {
  const UnaryBooleanTestData cases[] = {
      {"", false},    {"/", true},         {"foo/", true},
      {"bar", false}, {"/foo/bar", false},
  };
  for (const auto& i : cases) {
    FilePath input = FilePath(i.input).NormalizePathSeparators();
    EXPECT_EQ(i.expected, input.EndsWithSeparator());
  }
}

TEST(FilePathTest, AsEndingWithSeparator) {
  const UnaryTestData cases[] = {
      {"", ""}, {"/", "/"}, {"foo", "foo/"}, {"foo/", "foo/"}};
  for (const auto& i : cases) {
    FilePath input = FilePath(i.input).NormalizePathSeparators();
    FilePath expected = FilePath(i.expected).NormalizePathSeparators();
    EXPECT_EQ(expected.value(), input.AsEndingWithSeparator().value());
  }
}

// Test the operator<<(ostream, FilePath).
TEST(FilePathTest, PrintToOstream) {
  std::stringstream ss;
  FilePath fp("foo");
  ss << fp;
  EXPECT_EQ("foo", ss.str());
}

}  // namespace base
