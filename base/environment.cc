// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/environment.h"

#include "absl/strings/ascii.h"
#include "base/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
#include <stdlib.h>
#endif

namespace base {

namespace {

class EnvironmentImpl : public Environment {
 public:
  bool GetVar(absl::string_view variable_name, std::string* result) override {
    if (GetVarImpl(variable_name, result)) return true;

    // Some commonly used variable names are uppercase while others
    // are lowercase, which is inconsistent. Let's try to be helpful
    // and look for a variable name with the reverse case.
    // I.e. HTTP_PROXY may be http_proxy for some users/systems.
    char first_char = variable_name[0];
    std::string alternate_case_var;
    if (absl::ascii_islower(first_char))
      alternate_case_var = absl::AsciiStrToUpper(variable_name);
    else if (absl::ascii_isupper(first_char))
      alternate_case_var = absl::AsciiStrToLower(variable_name);
    else
      return false;
    return GetVarImpl(alternate_case_var, result);
  }

  bool SetVar(absl::string_view variable_name,
              const std::string& new_value) override {
    return SetVarImpl(variable_name, new_value);
  }

  bool UnSetVar(absl::string_view variable_name) override {
    return UnSetVarImpl(variable_name);
  }

 private:
  bool GetVarImpl(absl::string_view variable_name, std::string* result) {
#if defined(OS_WIN)
    DWORD value_length =
        ::GetEnvironmentVariable(variable_name.data(), nullptr, 0);
    if (value_length == 0) return false;
    if (result) {
      result->resize(value_length);
      ::GetEnvironmentVariable(variable_name.data(),
                               const_cast<char*>(result->c_str()),
                               value_length);
    }
    return true;
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
    const char* env_value = getenv(variable_name.data());
    if (!env_value) return false;
    // Note that the variable may be defined but empty.
    if (result) *result = env_value;
    return true;
#endif
  }

  bool SetVarImpl(absl::string_view variable_name,
                  const std::string& new_value) {
#if defined(OS_WIN)
    // On success, a nonzero value is returned.
    return !!SetEnvironmentVariable(variable_name.data(), new_value.c_str());
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
    // On success, zero is returned.
    return !setenv(variable_name.data(), new_value.c_str(), 1);
#endif
  }

  bool UnSetVarImpl(absl::string_view variable_name) {
#if defined(OS_WIN)
    // On success, a nonzero value is returned.
    return !!SetEnvironmentVariable(variable_name.data(), nullptr);
#elif defined(OS_POSIX) || defined(OS_FUCHSIA)
    // On success, zero is returned.
    return !unsetenv(variable_name.data());
#endif
  }
};

}  // namespace

namespace env_vars {

#if defined(OS_POSIX) || defined(OS_FUCHSIA)
// On Posix systems, this variable contains the location of the user's home
// directory. (e.g, /home/username/).
const char kHome[] = "HOME";
#endif

}  // namespace env_vars

Environment::~Environment() = default;

// static
std::unique_ptr<Environment> Environment::Create() {
  return std::make_unique<EnvironmentImpl>();
}

bool Environment::HasVar(absl::string_view variable_name) {
  return GetVar(variable_name, nullptr);
}

}  // namespace base
