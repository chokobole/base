// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_FILES_FILE_PATH_H_
#define BASE_FILES_FILE_PATH_H_

#include <string>
#include <vector>

#include "absl/strings/string_view.h"
#include "base/build_config.h"
#include "base/compiler_specific.h"
#include "base/export.h"

// Windows-style drive letter support and pathname separator characters can be
// enabled and disabled independently, to aid testing.  These #defines are
// here so that the same setting can be used in both the implementation and
// in the unit test.
#if defined(OS_WIN)
#define FILE_PATH_USES_DRIVE_LETTERS
#define FILE_PATH_USES_WIN_SEPARATORS
#endif  // OS_WIN

// To print path names portably use PRFilePath (based on PRIuS and friends from
// C99) like this:
// absl::StrFormat("Path is %" PRFilePath ".\n", path.value().c_str());
#if defined(OS_WIN)
#define PRFilePath "ls"
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
#define PRFilePath "s"
#endif  // OS_WIN

// Macros for string literal initialization.
#if defined(OS_WIN)
#define FILE_PATH_LITERAL(x) L##x
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
#define FILE_PATH_LITERAL(x) x
#endif  // OS_WIN

namespace base {

class BASE_EXPORT FilePath {
 public:
  // Null-terminated array of separators used to separate components in
  // hierarchical paths.  Each character in this array is a valid separator,
  // but kSeparators[0] is treated as the canonical separator and will be used
  // when composing pathnames.
  static const char kSeparators[];

  // base::size(kSeparators).
  static const size_t kSeparatorsLength;

  // A special path component meaning "this directory."
  static const char kCurrentDirectory[];

  // A special path component meaning "the parent directory."
  static const char kParentDirectory[];

  // The character used to identify a file extension.
  static const char kExtensionSeparator;

  FilePath();
  FilePath(const FilePath& that);
  explicit FilePath(absl::string_view path);
  ~FilePath();
  FilePath& operator=(const FilePath& that);

  // Constructs FilePath with the contents of |that|, which is left in valid but
  // unspecified state.
  FilePath(FilePath&& that) noexcept;
  // Replaces the contents with those of |that|, which is left in valid but
  // unspecified state.
  FilePath& operator=(FilePath&& that);

  bool operator==(const FilePath& that) const;

  bool operator!=(const FilePath& that) const;

  // Required for some STL containers and operations
  bool operator<(const FilePath& that) const { return path_ < that.path_; }

  const std::string& value() const { return path_; }

  bool empty() const { return path_.empty(); }

  void clear() { path_.clear(); }

  // Returns true if |character| is in kSeparators.
  static bool IsSeparator(char character);

  // Returns a vector of all of the components of the provided path. It is
  // equivalent to calling DirName().value() on the path's root component,
  // and BaseName().value() on each child component.
  //
  // To make sure this is lossless so we can differentiate absolute and
  // relative paths, the root slash will be included even though no other
  // slashes will be. The precise behavior is:
  //
  // Posix:  "/foo/bar"  ->  [ "/", "foo", "bar" ]
  // Windows:  "C:\foo\bar"  ->  [ "C:", "\\", "foo", "bar" ]
  void GetComponents(std::vector<std::string>* components) const;

  // Returns true if this FilePath is a parent or ancestor of the |child|.
  // Absolute and relative paths are accepted i.e. /foo is a parent to /foo/bar,
  // and foo is a parent to foo/bar. Any ancestor is considered a parent i.e. /a
  // is a parent to both /a/b and /a/b/c.  Does not convert paths to absolute,
  // follow symlinks or directory navigation (e.g. ".."). A path is *NOT* its
  // own parent.
  bool IsParent(const FilePath& child) const;

  // If IsParent(child) holds, appends to path (if non-NULL) the
  // relative path to child and returns true.  For example, if parent
  // holds "/Users/johndoe/Library/Application Support", child holds
  // "/Users/johndoe/Library/Application Support/Google/Chrome/Default", and
  // *path holds "/Users/johndoe/Library/Caches", then after
  // parent.AppendRelativePath(child, path) is called *path will hold
  // "/Users/johndoe/Library/Caches/Google/Chrome/Default".  Otherwise,
  // returns false.
  bool AppendRelativePath(const FilePath& child, FilePath* path) const;

  // Returns a FilePath corresponding to the directory containing the path
  // named by this object, stripping away the file component.  If this object
  // only contains one component, returns a FilePath identifying
  // kCurrentDirectory.  If this object already refers to the root directory,
  // returns a FilePath identifying the root directory. Please note that this
  // doesn't resolve directory navigation, e.g. the result for "../a" is "..".
  FilePath DirName() const WARN_UNUSED_RESULT;

  // Returns a FilePath corresponding to the last path component of this
  // object, either a file or a directory.  If this object already refers to
  // the root directory, returns a FilePath identifying the root directory;
  // this is the only situation in which BaseName will return an absolute path.
  FilePath BaseName() const WARN_UNUSED_RESULT;

  // Returns ".jpg" for path "C:\pics\jojo.jpg", or an empty string if
  // the file has no extension.  If non-empty, Extension() will always start
  // with precisely one ".".  The following code should always work regardless
  // of the value of path.
  // new_path = path.RemoveExtension().value().append(path.Extension());
  // ASSERT(new_path == path.value());
  // NOTE: this is different from the original file_util implementation which
  // returned the extension without a leading "." ("jpg" instead of ".jpg")
  std::string Extension() const WARN_UNUSED_RESULT;

  // Returns "C:\pics\jojo" for path "C:\pics\jojo.jpg"
  // NOTE: this is slightly different from the similar file_util implementation
  // which returned simply 'jojo'.
  FilePath RemoveExtension() const WARN_UNUSED_RESULT;

  // Adds |extension| to |file_name|. Returns the current FilePath if
  // |extension| is empty. Returns "" if BaseName() == "." or "..".
  FilePath AddExtension(absl::string_view extension) const WARN_UNUSED_RESULT;

  // Replaces the extension of |file_name| with |extension|.  If |file_name|
  // does not have an extension, then |extension| is added.  If |extension| is
  // empty, then the extension is removed from |file_name|.
  // Returns "" if BaseName() == "." or "..".
  FilePath ReplaceExtension(absl::string_view extension) const
      WARN_UNUSED_RESULT;

  // Returns a FilePath by appending a separator and the supplied path
  // component to this object's path.  Append takes care to avoid adding
  // excessive separators if this object's path already ends with a separator.
  // If this object's path is kCurrentDirectory, a new FilePath corresponding
  // only to |component| is returned.  |component| must be a relative path;
  // it is an error to pass an absolute path.
  FilePath Append(absl::string_view component) const WARN_UNUSED_RESULT;
  FilePath Append(const FilePath& component) const WARN_UNUSED_RESULT;

  // Returns true if this FilePath contains an absolute path.  On Windows, an
  // absolute path begins with either a drive letter specification followed by
  // a separator character, or with two separator characters.  On POSIX
  // platforms, an absolute path begins with a separator character.
  bool IsAbsolute() const;

  // Returns true if the patch ends with a path separator character.
  bool EndsWithSeparator() const WARN_UNUSED_RESULT;

  // Returns a copy of this FilePath that ends with a trailing separator. If
  // the input path is empty, an empty FilePath will be returned.
  FilePath AsEndingWithSeparator() const WARN_UNUSED_RESULT;

  // Returns a copy of this FilePath that does not end with a trailing
  // separator.
  FilePath StripTrailingSeparators() const WARN_UNUSED_RESULT;

  // Returns true if this FilePath contains an attempt to reference a parent
  // directory (e.g. has a path component that is "..").
  bool ReferencesParent() const;

  // Normalize all path separators to backslash on Windows
  // (if FILE_PATH_USES_WIN_SEPARATORS is true), or do nothing on POSIX systems.
  FilePath NormalizePathSeparators() const;

  // Normalize all path separattors to given type on Windows
  // (if FILE_PATH_USES_WIN_SEPARATORS is true), or do nothing on POSIX systems.
  FilePath NormalizePathSeparatorsTo(char separator) const;

#if defined(OS_ANDROID)
  // On android, file selection dialog can return a file with content uri
  // scheme(starting with content://). Content uri needs to be opened with
  // ContentResolver to guarantee that the app has appropriate permissions
  // to access it.
  // Returns true if the path is a content uri, or false otherwise.
  bool IsContentUri() const;
#endif

 private:
  // Remove trailing separators from this object.  If the path is absolute, it
  // will never be stripped any more than to refer to the absolute root
  // directory, so "////" will become "/", not "".  A leading pair of
  // separators is never stripped, to support alternate roots.  This is used to
  // support UNC paths on Windows.
  void StripTrailingSeparatorsInternal();

  std::string path_;
};

BASE_EXPORT std::ostream& operator<<(std::ostream& out,
                                     const FilePath& file_path);

}  // namespace base

namespace std {

template <>
struct hash<base::FilePath> {
  std::size_t operator()(const base::FilePath& f) const {
    return hash<std::string>()(f.value());
  }
};

}  // namespace std

#endif  // BASE_FILES_FILE_PATH_H_