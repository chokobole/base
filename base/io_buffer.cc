// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/io_buffer.h"

#include "base/logging.h"

namespace base {

IOBuffer::IOBuffer() : data_(nullptr) {}

IOBuffer::IOBuffer(size_t buffer_size) : data_(new char[buffer_size]) {}

IOBuffer::IOBuffer(char* data) : data_(data) {}

IOBuffer::~IOBuffer() {
  delete[] data_;
  data_ = nullptr;
}

IOBufferWithSize::IOBufferWithSize(size_t size) : IOBuffer(size), size_(size) {}

IOBufferWithSize::IOBufferWithSize(char* data, size_t size)
    : IOBuffer(data), size_(size) {}

IOBufferWithSize::~IOBufferWithSize() = default;

StringIOBuffer::StringIOBuffer(const std::string& s)
    : IOBuffer(static_cast<char*>(nullptr)), string_data_(s) {
  data_ = const_cast<char*>(string_data_.data());
}

StringIOBuffer::StringIOBuffer(std::unique_ptr<std::string> s)
    : IOBuffer(static_cast<char*>(nullptr)) {
  string_data_.swap(*s.get());
  data_ = const_cast<char*>(string_data_.data());
}

StringIOBuffer::~StringIOBuffer() {
  // We haven't allocated the buffer, so remove it before the base class
  // destructor tries to delete[] it.
  data_ = nullptr;
}

DrainableIOBuffer::DrainableIOBuffer(std::shared_ptr<IOBuffer> base,
                                     size_t size)
    : IOBuffer(base->data()), base_(std::move(base)), size_(size), used_(0) {}

void DrainableIOBuffer::DidConsume(size_t bytes) { SetOffset(used_ + bytes); }

size_t DrainableIOBuffer::BytesRemaining() const { return size_ - used_; }

// Returns the number of consumed bytes.
size_t DrainableIOBuffer::BytesConsumed() const { return used_; }

void DrainableIOBuffer::SetOffset(size_t bytes) {
  DCHECK_LE(bytes, size_);
  used_ = bytes;
  data_ = base_->data() + used_;
}

DrainableIOBuffer::~DrainableIOBuffer() {
  // The buffer is owned by the |base_| instance.
  data_ = nullptr;
}

GrowableIOBuffer::GrowableIOBuffer() : IOBuffer(), capacity_(0), offset_(0) {}

void GrowableIOBuffer::SetCapacity(size_t capacity) {
  // realloc will crash if it fails.
  real_data_.reset(static_cast<char*>(realloc(real_data_.release(), capacity)));
  capacity_ = capacity;
  if (offset_ > capacity)
    SetOffset(capacity);
  else
    SetOffset(offset_);  // The pointer may have changed.
}

void GrowableIOBuffer::SetOffset(size_t offset) {
  DCHECK_LE(offset, capacity_);
  offset_ = offset;
  data_ = real_data_.get() + offset;
}

size_t GrowableIOBuffer::RemainingCapacity() { return capacity_ - offset_; }

char* GrowableIOBuffer::StartOfBuffer() { return real_data_.get(); }

GrowableIOBuffer::~GrowableIOBuffer() { data_ = nullptr; }

WrappedIOBuffer::WrappedIOBuffer(const char* data)
    : IOBuffer(const_cast<char*>(data)) {}

WrappedIOBuffer::~WrappedIOBuffer() { data_ = nullptr; }

}  // namespace base
