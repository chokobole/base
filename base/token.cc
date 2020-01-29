// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/token.h"

#include <inttypes.h>

#include "absl/random/random.h"
#include "absl/strings/str_format.h"

namespace base {

// static
Token Token::CreateRandom() {
  Token token;

  absl::BitGen bitgen;
  token.high_ = absl::Uniform<uint64_t>(bitgen);
  token.low_ = absl::Uniform<uint64_t>(bitgen);
  return token;
}

std::string Token::ToString() const {
  return absl::StrFormat("%016" PRIX64 "%016" PRIX64, high_, low_);
}

}  // namespace base
