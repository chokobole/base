// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/file_path.h"

#include <string.h>

#include <algorithm>

#include "absl/strings/ascii.h"
#include "absl/strings/str_cat.h"
#include "base/logging.h"

namespace base {

namespace {

const char kStringTerminator = '\0';

// If this FilePath contains a drive letter specification, returns the
// position of the last character of the drive letter specification,
// otherwise returns npos.  This can only be true on Windows, when a pathname
// begins with a letter followed by a colon.  On other platforms, this always
// returns npos.
size_t FindDriveLetter(absl::string_view path) {
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
  // This is dependent on an ASCII-based character set, but that's a
  // reasonable assumption.  iswalpha can be too inclusive here.
  if (path.length() >= 2 && path[1] == ':' && absl::ascii_isalpha(path[0])) {
    return 1;
  }
#endif  // FILE_PATH_USES_DRIVE_LETTERS

  return std::string::npos;
}

#if defined(FILE_PATH_USES_DRIVE_LETTERS)
bool EqualDriveLetterCaseInsensitive(absl::string_view a, absl::string_view b) {
  size_t a_letter_pos = FindDriveLetter(a);
  size_t b_letter_pos = FindDriveLetter(b);

  if (a_letter_pos == std::string::npos || b_letter_pos == std::string::npos)
    return a == b;

  absl::string_view a_letter(a.substr(0, a_letter_pos + 1));
  absl::string_view b_letter(b.substr(0, b_letter_pos + 1));
  if (absl::ascii_toupper(a_letter[0]) != absl::ascii_toupper(b_letter[0]))
    return false;

  absl::string_view a_rest(a.substr(a_letter_pos + 1));
  absl::string_view b_rest(b.substr(b_letter_pos + 1));
  return a_rest == b_rest;
}
#endif  // defined(FILE_PATH_USES_DRIVE_LETTERS)

bool IsPathAbsolute(absl::string_view path) {
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
  size_t letter = FindDriveLetter(path);
  if (letter != std::string::npos) {
    // Look for a separator right after the drive specification.
    return path.length() > letter + 1 &&
           FilePath::IsSeparator(path[letter + 1]);
  }
  // Look for a pair of leading separators.
  return path.length() > 1 && FilePath::IsSeparator(path[0]) &&
         FilePath::IsSeparator(path[1]);
#else   // FILE_PATH_USES_DRIVE_LETTERS
  // Look for a separator in the first position.
  return path.length() > 0 && FilePath::IsSeparator(path[0]);
#endif  // FILE_PATH_USES_DRIVE_LETTERS
}

bool AreAllSeparators(const std::string& input) {
  for (auto it : input) {
    if (!FilePath::IsSeparator(it)) return false;
  }

  return true;
}

// Find the position of the '.' that separates the extension from the rest
// of the file name. The position is relative to BaseName(), not value().
// Returns npos if it can't find an extension.
size_t ExtensionSeparatorPosition(const std::string& path) {
  // Special case "." and ".."
  if (path == FilePath::kCurrentDirectory || path == FilePath::kParentDirectory)
    return std::string::npos;

  return path.rfind(FilePath::kExtensionSeparator);
}

// Returns true if path is "", ".", or "..".
bool IsEmptyOrSpecialCase(const std::string& path) {
  // Special cases "", ".", and ".."
  if (path.empty() || path == FilePath::kCurrentDirectory ||
      path == FilePath::kParentDirectory) {
    return true;
  }

  return false;
}

}  // namespace

FilePath::FilePath() = default;

FilePath::FilePath(const FilePath& that) = default;
FilePath::FilePath(FilePath&& that) noexcept = default;

FilePath::FilePath(absl::string_view path) : path_(std::string(path)) {
  size_t nul_pos = path_.find(kStringTerminator);
  if (nul_pos != std::string::npos) path_.erase(nul_pos, std::string::npos);
}

FilePath::~FilePath() = default;

FilePath& FilePath::operator=(const FilePath& that) = default;

FilePath& FilePath::operator=(FilePath&& that) = default;

bool FilePath::operator==(const FilePath& that) const {
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
  return EqualDriveLetterCaseInsensitive(this->path_, that.path_);
#else   // defined(FILE_PATH_USES_DRIVE_LETTERS)
  return path_ == that.path_;
#endif  // defined(FILE_PATH_USES_DRIVE_LETTERS)
}

bool FilePath::operator!=(const FilePath& that) const {
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
  return !EqualDriveLetterCaseInsensitive(this->path_, that.path_);
#else   // defined(FILE_PATH_USES_DRIVE_LETTERS)
  return path_ != that.path_;
#endif  // defined(FILE_PATH_USES_DRIVE_LETTERS)
}

std::ostream& operator<<(std::ostream& out, const FilePath& file_path) {
  return out << file_path.value();
}

// static
bool FilePath::IsSeparator(char character) {
  for (size_t i = 0; i < kSeparatorsLength - 1; ++i) {
    if (character == kSeparators[i]) {
      return true;
    }
  }

  return false;
}

void FilePath::GetComponents(std::vector<std::string>* components) const {
  DCHECK(components);
  if (!components) return;
  components->clear();
  if (value().empty()) return;

  std::vector<std::string> ret_val;
  FilePath current = *this;
  FilePath base;

  // Capture path components.
  while (current != current.DirName()) {
    base = current.BaseName();
    if (!AreAllSeparators(base.value())) ret_val.push_back(base.value());
    current = current.DirName();
  }

  // Capture root, if any.
  base = current.BaseName();
  if (!base.value().empty() && base.value() != kCurrentDirectory)
    ret_val.push_back(current.BaseName().value());

  // Capture drive letter, if any.
  FilePath dir = current.DirName();
  size_t letter = FindDriveLetter(dir.value());
  if (letter != std::string::npos) {
    ret_val.push_back(std::string(dir.value(), 0, letter + 1));
  }

  *components = std::vector<std::string>(ret_val.rbegin(), ret_val.rend());
}

// libgen's dirname and basename aren't guaranteed to be thread-safe and aren't
// guaranteed to not modify their input strings, and in fact are implemented
// differently in this regard on different platforms.  Don't use them, but
// adhere to their behavior.
FilePath FilePath::DirName() const {
  FilePath new_path(path_);
  new_path.StripTrailingSeparatorsInternal();

  // The drive letter, if any, always needs to remain in the output.  If there
  // is no drive letter, as will always be the case on platforms which do not
  // support drive letters, letter will be npos, or -1, so the comparisons and
  // resizes below using letter will still be valid.
  size_t letter = FindDriveLetter(new_path.path_);

  size_t last_separator = new_path.path_.find_last_of(
      kSeparators, std::string::npos, kSeparatorsLength - 1);
  if (last_separator == std::string::npos) {
    // path_ is in the current directory.
    new_path.path_.resize(letter + 1);
  } else if (last_separator == letter + 1) {
    // path_ is in the root directory.
    new_path.path_.resize(letter + 2);
  } else if (last_separator == letter + 2 &&
             IsSeparator(new_path.path_[letter + 1])) {
    // path_ is in "//" (possibly with a drive letter); leave the double
    // separator intact indicating alternate root.
    new_path.path_.resize(letter + 3);
  } else if (last_separator != 0) {
    // path_ is somewhere else, trim the basename.
    new_path.path_.resize(last_separator);
  }

  new_path.StripTrailingSeparatorsInternal();
  if (!new_path.path_.length()) new_path.path_ = kCurrentDirectory;

  return new_path;
}

FilePath FilePath::BaseName() const {
  FilePath new_path(path_);
  new_path.StripTrailingSeparatorsInternal();

  // The drive letter, if any, is always stripped.
  size_t letter = FindDriveLetter(new_path.path_);
  if (letter != std::string::npos) {
    new_path.path_.erase(0, letter + 1);
  }

  // Keep everything after the final separator, but if the pathname is only
  // one character and it's a separator, leave it alone.
  size_t last_separator = new_path.path_.find_last_of(
      kSeparators, std::string::npos, kSeparatorsLength - 1);
  if (last_separator != std::string::npos &&
      last_separator < new_path.path_.length() - 1) {
    new_path.path_.erase(0, last_separator + 1);
  }

  return new_path;
}

std::string FilePath::Extension() const {
  FilePath base(BaseName());
  const size_t dot = ExtensionSeparatorPosition(base.path_);
  if (dot == std::string::npos) return std::string();

  return base.path_.substr(dot, std::string::npos);
}

FilePath FilePath::RemoveExtension() const {
  if (Extension().empty()) return *this;

  const size_t dot = ExtensionSeparatorPosition(path_);
  if (dot == std::string::npos) return *this;

  return FilePath(path_.substr(0, dot));
}

FilePath FilePath::AddExtension(absl::string_view extension) const {
  if (IsEmptyOrSpecialCase(BaseName().value())) return FilePath();

  // If the new extension is "" or ".", then just return the current FilePath.
  if (extension.empty() ||
      (extension.size() == 1 && extension[0] == kExtensionSeparator))
    return *this;

  std::string str = path_;
  if (extension[0] != kExtensionSeparator &&
      *(str.end() - 1) != kExtensionSeparator) {
    str.append(1, kExtensionSeparator);
  }
  absl::StrAppend(&str, extension);
  return FilePath(str);
}

FilePath FilePath::ReplaceExtension(absl::string_view extension) const {
  if (IsEmptyOrSpecialCase(BaseName().value())) return FilePath();

  FilePath no_ext = RemoveExtension();
  // If the new extension is "" or ".", then just remove the current extension.
  if (extension.empty() ||
      (extension.size() == 1 && extension[0] == kExtensionSeparator))
    return no_ext;

  std::string str = no_ext.value();
  if (extension[0] != kExtensionSeparator) str.append(1, kExtensionSeparator);
  absl::StrAppend(&str, extension);
  return FilePath(str);
}

FilePath FilePath::Append(absl::string_view component) const {
  absl::string_view appended = component;
  std::string without_nuls;

  size_t nul_pos = component.find(kStringTerminator);
  if (nul_pos != absl::string_view::npos) {
    without_nuls = std::string(component.substr(0, nul_pos));
    appended = absl::string_view(without_nuls);
  }

  DCHECK(!IsPathAbsolute(appended));

  if (path_.compare(kCurrentDirectory) == 0 && !appended.empty()) {
    // Append normally doesn't do any normalization, but as a special case,
    // when appending to kCurrentDirectory, just return a new path for the
    // component argument.  Appending component to kCurrentDirectory would
    // serve no purpose other than needlessly lengthening the path, and
    // it's likely in practice to wind up with FilePath objects containing
    // only kCurrentDirectory when calling DirName on a single relative path
    // component.
    return FilePath(appended);
  }

  FilePath new_path(path_);
  new_path.StripTrailingSeparatorsInternal();

  // Don't append a separator if the path is empty (indicating the current
  // directory) or if the path component is empty (indicating nothing to
  // append).
  if (!appended.empty() && !new_path.path_.empty()) {
    // Don't append a separator if the path still ends with a trailing
    // separator after stripping (indicating the root directory).
    if (!IsSeparator(new_path.path_.back())) {
      // Don't append a separator if the path is just a drive letter.
      if (FindDriveLetter(new_path.path_) + 1 != new_path.path_.length()) {
        new_path.path_.append(1, kSeparators[0]);
      }
    }
  }
  absl::StrAppend(&new_path.path_, appended);
  return new_path;
}

FilePath FilePath::Append(const FilePath& component) const {
  return Append(component.value());
}

bool FilePath::IsAbsolute() const { return IsPathAbsolute(path_); }

bool FilePath::EndsWithSeparator() const {
  if (empty()) return false;
  return IsSeparator(path_.back());
}

FilePath FilePath::AsEndingWithSeparator() const {
  if (EndsWithSeparator() || path_.empty()) return *this;

  std::string path_str;
  path_str.reserve(path_.length() + 1);  // Only allocate string once.

  path_str = path_;
  path_str.append(&kSeparators[0], 1);
  return FilePath(path_str);
}

FilePath FilePath::StripTrailingSeparators() const {
  FilePath new_path(path_);
  new_path.StripTrailingSeparatorsInternal();

  return new_path;
}

bool FilePath::ReferencesParent() const {
  if (path_.find(kParentDirectory) == std::string::npos) {
    // GetComponents is quite expensive, so avoid calling it in the majority
    // of cases where there isn't a kParentDirectory anywhere in the path.
    return false;
  }

  std::vector<std::string> components;
  GetComponents(&components);

  std::vector<std::string>::const_iterator it = components.begin();
  for (; it != components.end(); ++it) {
    const std::string& component = *it;
    // Windows has odd, undocumented behavior with path components containing
    // only whitespace and . characters. So, if all we see is . and
    // whitespace, then we treat any .. sequence as referencing parent.
    // For simplicity we enforce this on all platforms.
    if (component.find_first_not_of(". \n\r\t") == std::string::npos &&
        component.find(kParentDirectory) != std::string::npos) {
      return true;
    }
  }
  return false;
}

void FilePath::StripTrailingSeparatorsInternal() {
  // If there is no drive letter, start will be 1, which will prevent stripping
  // the leading separator if there is only one separator.  If there is a drive
  // letter, start will be set appropriately to prevent stripping the first
  // separator following the drive letter, if a separator immediately follows
  // the drive letter.
  size_t start = FindDriveLetter(path_) + 2;

  size_t last_stripped = std::string::npos;
  for (size_t pos = path_.length(); pos > start && IsSeparator(path_[pos - 1]);
       --pos) {
    // If the string only has two separators and they're at the beginning,
    // don't strip them, unless the string began with more than two separators.
    if (pos != start + 1 || last_stripped == start + 2 ||
        !IsSeparator(path_[start - 1])) {
      path_.resize(pos - 1);
      last_stripped = pos;
    }
  }
}

FilePath FilePath::NormalizePathSeparators() const {
  return NormalizePathSeparatorsTo(kSeparators[0]);
}

FilePath FilePath::NormalizePathSeparatorsTo(char separator) const {
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
  DCHECK_NE(kSeparators + kSeparatorsLength,
            std::find(kSeparators, kSeparators + kSeparatorsLength, separator));
  std::string copy = path_;
  for (size_t i = 0; i < kSeparatorsLength; ++i) {
    std::replace(copy.begin(), copy.end(), kSeparators[i], separator);
  }
  return FilePath(copy);
#else
  return *this;
#endif
}

}  // namespace base
