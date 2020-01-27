// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/file_util.h"

#if defined(OS_WIN)
#include <io.h>
#endif
#include <stdio.h>

#include <fstream>
#include <limits>
#include <memory>

#include "base/files/file_enumerator.h"
#include "base/logging.h"
#include "base/strings/string_util.h"

namespace base {

bool Move(const FilePath& from_path, const FilePath& to_path) {
  if (from_path.ReferencesParent() || to_path.ReferencesParent()) return false;
  return internal::MoveUnsafe(from_path, to_path);
}

bool ContentsEqual(const FilePath& filename1, const FilePath& filename2) {
  // We open the file in binary format even if they are text files because
  // we are just comparing that bytes are exactly same in both files and not
  // doing anything smart with text formatting.
  std::ifstream file1(filename1.value(), std::ios::in | std::ios::binary);
  std::ifstream file2(filename2.value(), std::ios::in | std::ios::binary);

  // Even if both files aren't openable (and thus, in some sense, "equal"),
  // any unusable file yields a result of "false".
  if (!file1.is_open() || !file2.is_open()) return false;

  const int BUFFER_SIZE = 2056;
  char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
  do {
    file1.read(buffer1, BUFFER_SIZE);
    file2.read(buffer2, BUFFER_SIZE);

    if ((file1.eof() != file2.eof()) || (file1.gcount() != file2.gcount()) ||
        (memcmp(buffer1, buffer2, static_cast<size_t>(file1.gcount())))) {
      file1.close();
      file2.close();
      return false;
    }
  } while (!file1.eof() || !file2.eof());

  file1.close();
  file2.close();
  return true;
}

bool TextContentsEqual(const FilePath& filename1, const FilePath& filename2) {
  std::ifstream file1(filename1.value(), std::ios::in);
  std::ifstream file2(filename2.value(), std::ios::in);

  // Even if both files aren't openable (and thus, in some sense, "equal"),
  // any unusable file yields a result of "false".
  if (!file1.is_open() || !file2.is_open()) return false;

  do {
    std::string line1, line2;
    getline(file1, line1);
    getline(file2, line2);

    // Check for mismatched EOF states, or any error state.
    if ((file1.eof() != file2.eof()) || file1.bad() || file2.bad()) {
      return false;
    }

    // Trim all '\r' and '\n' characters from the end of the line.
    std::string::size_type end1 = line1.find_last_not_of("\r\n");
    if (end1 == std::string::npos)
      line1.clear();
    else if (end1 + 1 < line1.length())
      line1.erase(end1 + 1);

    std::string::size_type end2 = line2.find_last_not_of("\r\n");
    if (end2 == std::string::npos)
      line2.clear();
    else if (end2 + 1 < line2.length())
      line2.erase(end2 + 1);

    if (line1 != line2) return false;
  } while (!file1.eof() || !file2.eof());

  return true;
}

bool ReadFileToStringWithMaxSize(const FilePath& path, std::string* contents,
                                 size_t max_size) {
  if (contents) contents->clear();
  if (path.ReferencesParent()) return false;
  FILE* file = OpenFile(path, "rb");
  if (!file) {
    return false;
  }

  // Many files supplied in |path| have incorrect size (proc files etc).
  // Hence, the file is read sequentially as opposed to a one-shot read, using
  // file size as a hint for chunk size if available.
  constexpr int64_t kDefaultChunkSize = 1 << 16;
  int64_t chunk_size;
  if (!GetFileSize(path, &chunk_size) || chunk_size <= 0)
    chunk_size = kDefaultChunkSize - 1;
  // We need to attempt to read at EOF for feof flag to be set so here we
  // use |chunk_size| + 1.
  chunk_size = std::min<uint64_t>(chunk_size, max_size) + 1;
  size_t bytes_read_this_pass;
  size_t bytes_read_so_far = 0;
  bool read_status = true;
  std::string local_contents;
  local_contents.resize(chunk_size);

  while ((bytes_read_this_pass = fread(&local_contents[bytes_read_so_far], 1,
                                       chunk_size, file)) > 0) {
    if ((max_size - bytes_read_so_far) < bytes_read_this_pass) {
      // Read more than max_size bytes, bail out.
      bytes_read_so_far = max_size;
      read_status = false;
      break;
    }
    // In case EOF was not reached, iterate again but revert to the default
    // chunk size.
    if (bytes_read_so_far == 0) chunk_size = kDefaultChunkSize;

    bytes_read_so_far += bytes_read_this_pass;
    // Last fread syscall (after EOF) can be avoided via feof, which is just a
    // flag check.
    if (feof(file)) break;
    local_contents.resize(bytes_read_so_far + chunk_size);
  }
  read_status = read_status && !ferror(file);
  CloseFile(file);
  if (contents) {
    contents->swap(local_contents);
    contents->resize(bytes_read_so_far);
  }

  return read_status;
}

bool ReadFileToString(const FilePath& path, std::string* contents) {
  return ReadFileToStringWithMaxSize(path, contents,
                                     std::numeric_limits<size_t>::max());
}

bool IsDirectoryEmpty(const FilePath& dir_path) {
  FileEnumerator files(dir_path, false,
                       FileEnumerator::FILES | FileEnumerator::DIRECTORIES);
  if (files.Next().empty()) return true;
  return false;
}

bool CreateDirectory(const FilePath& full_path) {
  return CreateDirectoryAndGetError(full_path, nullptr);
}

bool GetFileSize(const FilePath& file_path, int64_t* file_size) {
  File::Info info;
  if (!GetFileInfo(file_path, &info)) return false;
  *file_size = info.size;
  return true;
}

bool TouchFile(const FilePath& path, const absl::Time& last_accessed,
               const absl::Time& last_modified) {
  int flags = File::FLAG_OPEN | File::FLAG_WRITE_ATTRIBUTES;

#if defined(OS_WIN)
  // On Windows, FILE_FLAG_BACKUP_SEMANTICS is needed to open a directory.
  if (DirectoryExists(path)) flags |= File::FLAG_BACKUP_SEMANTICS;
#elif defined(OS_FUCHSIA)
  // On Fuchsia, we need O_RDONLY for directories, or O_WRONLY for files.
  // TODO(https://crbug.com/947802): Find a cleaner workaround for this.
  flags |= (DirectoryExists(path) ? File::FLAG_READ : File::FLAG_WRITE);
#endif

  File file(path, flags);
  if (!file.IsValid()) return false;

  return file.SetTimes(last_accessed, last_modified);
}

bool CloseFile(FILE* file) {
  if (file == nullptr) return true;
  return fclose(file) == 0;
}

bool TruncateFile(FILE* file) {
  if (file == nullptr) return false;
  long current_offset = ftell(file);
  if (current_offset == -1) return false;
#if defined(OS_WIN)
  int fd = _fileno(file);
  if (_chsize(fd, current_offset) != 0) return false;
#else
  int fd = fileno(file);
  if (ftruncate(fd, current_offset) != 0) return false;
#endif
  return true;
}

}  // namespace base
