// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/data_view.h"

namespace base {

ConstDataView::ConstDataView(const char* data, size_t length)
    : data_(data), length_(length) {}

ConstDataView::ConstDataView(const ConstDataView& other) noexcept = default;

ConstDataView& ConstDataView::operator=(const ConstDataView& other) noexcept =
    default;

size_t ConstDataView::length() const { return length_; }

DataView::DataView(char* data, size_t length) : data_(data), length_(length) {}

DataView::DataView(const DataView& other) noexcept = default;

DataView& DataView::operator=(const DataView& other) noexcept = default;

size_t DataView::length() const { return length_; }

}  // namespace base