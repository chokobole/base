// Copyright (c) 2019 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_EXPORT_H_
#define BASE_EXPORT_H_

#if defined(BASE_COMPONENT_BUILD)

#if defined(_WIN32)
#ifdef BASE_COMPILE_LIBRARY
#define BASE_EXPORT __declspec(dllexport)
#else
#define BASE_EXPORT __declspec(dllimport)
#endif  // defined(BASE_COMPILE_LIBRARY)

#else
#ifdef BASE_COMPILE_LIBRARY
#define BASE_EXPORT __attribute__((visibility("default")))
#else
#define BASE_EXPORT
#endif  // defined(BASE_COMPILE_LIBRARY)
#endif  // defined(_WIN32)

#else
#define BASE_EXPORT
#endif  // defined(BASE_COMPONENT_BUILD)

#endif  // BASE_EXPORT_H_