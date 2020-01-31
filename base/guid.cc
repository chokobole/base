// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/guid.h"

#include <stddef.h>
#include <stdint.h>

#include "absl/random/random.h"
#include "absl/strings/ascii.h"
#include "absl/strings/str_format.h"

namespace base {

namespace {

bool IsLowerHexDigit(char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

bool IsValidGUIDInternal(absl::string_view guid, bool strict) {
  const size_t kGUIDLength = 36U;
  if (guid.length() != kGUIDLength) return false;

  for (size_t i = 0; i < guid.length(); ++i) {
    char current = guid[i];
    if (i == 8 || i == 13 || i == 18 || i == 23) {
      if (current != '-') return false;
    } else {
      if (strict ? !IsLowerHexDigit(current) : !absl::ascii_isxdigit(current))
        return false;
    }
  }

  return true;
}

}  // namespace

std::string GenerateGUID() {
  absl::BitGen bitgen;
  uint64_t sixteen_bytes[2];
  sixteen_bytes[0] = absl::Uniform<uint64_t>(bitgen);
  sixteen_bytes[1] = absl::Uniform<uint64_t>(bitgen);

  // Set the GUID to version 4 as described in RFC 4122, section 4.4.
  // The format of GUID version 4 must be xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx,
  // where y is one of [8, 9, A, B].

  // Clear the version bits and set the version to 4:
  sixteen_bytes[0] &= 0xffffffff'ffff0fffULL;
  sixteen_bytes[0] |= 0x00000000'00004000ULL;

  // Set the two most significant bits (bits 6 and 7) of the
  // clock_seq_hi_and_reserved to zero and one, respectively:
  sixteen_bytes[1] &= 0x3fffffff'ffffffffULL;
  sixteen_bytes[1] |= 0x80000000'00000000ULL;

  return RandomDataToGUIDString(sixteen_bytes);
}

bool IsValidGUID(absl::string_view guid) {
  return IsValidGUIDInternal(guid, false /* strict */);
}

bool IsValidGUIDOutputString(absl::string_view guid) {
  return IsValidGUIDInternal(guid, true /* strict */);
}

std::string RandomDataToGUIDString(const uint64_t bytes[2]) {
  return absl::StrFormat(
      "%08x-%04x-%04x-%04x-%012llx", static_cast<unsigned int>(bytes[0] >> 32),
      static_cast<unsigned int>((bytes[0] >> 16) & 0x0000ffff),
      static_cast<unsigned int>(bytes[0] & 0x0000ffff),
      static_cast<unsigned int>(bytes[1] >> 48),
      bytes[1] & 0x0000ffff'ffffffffULL);
}

}  // namespace base
