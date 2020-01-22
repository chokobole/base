// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CALLBACK_H_
#define BASE_CALLBACK_H_

#include <functional>
#include <type_traits>

#include "absl/functional/function_ref.h"
#include "base/callback_forward.h"

namespace base {

template <typename R, typename... Args>
class OnceCallback<R(Args...)> {
 public:
  typedef std::function<R(Args...)> CallbackTy;

  constexpr OnceCallback() = default;
  OnceCallback(std::nullptr_t) = delete;
  OnceCallback(absl::FunctionRef<R(Args...)> callback) : callback_(callback) {}
  template <
      typename T,
      std::enable_if_t<std::is_convertible<T, CallbackTy>::value>* = nullptr>
  OnceCallback(T&& callback) : callback_(callback) {}
  OnceCallback(const OnceCallback& other) = delete;
  OnceCallback& operator=(const OnceCallback& other) = delete;
  OnceCallback(OnceCallback&& other) noexcept = default;
  OnceCallback& operator=(OnceCallback&& other) noexcept = default;

  R Run(Args... args) && {
    CallbackTy callback = callback_;
    callback_ = nullptr;
    return callback(std::forward<Args>(args)...);
  }

  operator bool() { return static_cast<bool>(callback_); }

  bool is_null() const { return !static_cast<bool>(callback_); }

  void Reset() { callback_ = nullptr; }

 private:
  CallbackTy callback_;
};

template <typename R, typename... Args>
class RepeatingCallback<R(Args...)> {
 public:
  typedef std::function<R(Args...)> CallbackTy;

  constexpr RepeatingCallback() = default;
  RepeatingCallback(std::nullptr_t) = delete;
  RepeatingCallback(absl::FunctionRef<R(Args...)> callback)
      : callback_(callback) {}
  template <
      typename T,
      std::enable_if_t<std::is_convertible<T, CallbackTy>::value>* = nullptr>
  RepeatingCallback(T&& callback) : callback_(callback) {}
  RepeatingCallback(const RepeatingCallback& other) = default;
  RepeatingCallback& operator=(const RepeatingCallback& other) = default;

  R Run(Args... args) const& { return callback_(std::forward<Args>(args)...); }

  R Run(Args... args) && {
    CallbackTy callback = callback_;
    callback_ = nullptr;
    return callback(std::forward<Args>(args)...);
  }

  operator bool() { return static_cast<bool>(callback_); }

  bool is_null() const { return !static_cast<bool>(callback_); }

  void Reset() { callback_ = nullptr; }

 private:
  CallbackTy callback_;
};

}  // namespace base

#endif  // BASE_CALLBACK_H_