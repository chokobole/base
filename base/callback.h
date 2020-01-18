// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CALLBACK_H_
#define BASE_CALLBACK_H_

#include <functional>

using namespace std::placeholders;

template <typename R, typename... Args>
class OnceCallback;

template <typename R, typename... Args>
class OnceCallback<R(Args...)> {
 public:
  typedef std::function<R(Args...)> CallbackTy;

  OnceCallback() = default;
  explicit OnceCallback(CallbackTy callback) : callback_(callback) {}
  OnceCallback(const OnceCallback& other) = default;
  OnceCallback& operator=(const OnceCallback& other) = default;

  R Invoke(Args... args) && {
    CallbackTy callback = callback_;
    callback_ = nullptr;
    return callback(std::forward<Args>(args)...);
  }

  operator bool() { return static_cast<bool>(callback_); }

  bool is_null() const { return !static_cast<bool>(callback_); }

  void Reset() { callback_ = nullptr; }

  CallbackTy callback_;
};

template <typename R, typename... Args>
class RepeatingCallback;

template <typename R, typename... Args>
class RepeatingCallback<R(Args...)> {
 public:
  typedef std::function<R(Args...)> CallbackTy;

  RepeatingCallback() = default;
  explicit RepeatingCallback(CallbackTy callback) : callback_(callback) {}
  RepeatingCallback(const RepeatingCallback& other) = default;
  RepeatingCallback& operator=(const RepeatingCallback& other) = default;

  R Invoke(Args... args) const& {
    return callback_(std::forward<Args>(args)...);
  }

  R Invoke(Args... args) && {
    CallbackTy callback = callback_;
    callback_ = nullptr;
    return callback(std::forward<Args>(args)...);
  }

  operator bool() { return static_cast<bool>(callback_); }

  bool is_null() const { return !static_cast<bool>(callback_); }

  void Reset() { callback_ = nullptr; }

  CallbackTy callback_;
};

#endif  // BASE_CALLBACK_H_