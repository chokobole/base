// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_IO_BUFFER_H_
#define BASE_IO_BUFFER_H_

#include <stddef.h>

#include <memory>
#include <string>

#include "base/export.h"
#include "base/memory/free_deleter.h"

namespace base {

class BASE_EXPORT IOBuffer {
 public:
  IOBuffer();
  virtual ~IOBuffer();

  explicit IOBuffer(size_t buffer_size);

  char* data() const { return data_; }

 protected:
  // Only allow derived classes to specify data_.
  // In all other cases, we own data_, and must delete it at destruction time.
  explicit IOBuffer(char* data);

  char* data_;
};

// This version stores the size of the buffer so that the creator of the object
// doesn't have to keep track of that value.
class BASE_EXPORT IOBufferWithSize : public IOBuffer {
 public:
  explicit IOBufferWithSize(size_t size);
  ~IOBufferWithSize() override;

  size_t size() const { return size_; }

 protected:
  // Purpose of this constructor is to give a subclass access to the base class
  // constructor IOBuffer(char*) thus allowing subclass to use underlying
  // memory it does not own.
  IOBufferWithSize(char* data, size_t size);

  size_t size_;
};

// This is a read only IOBuffer.  The data is stored in a string and
// the IOBuffer interface does not provide a proper way to modify it.
class BASE_EXPORT StringIOBuffer : public IOBuffer {
 public:
  explicit StringIOBuffer(const std::string& s);
  explicit StringIOBuffer(std::unique_ptr<std::string> s);
  ~StringIOBuffer() override;

  size_t size() const { return string_data_.size(); }

 private:
  std::string string_data_;
};

// This version wraps an existing IOBuffer and provides convenient functions
// to progressively read all the data.
//
// DrainableIOBuffer is useful when you have an IOBuffer that contains data
// to be written progressively, and Write() function takes an IOBuffer rather
// than char*. DrainableIOBuffer can be used as follows:
//
// // payload is the IOBuffer containing the data to be written.
// buf = std::make_shared<DrainableIOBuffer>(payload, payload_size);
//
// while (buf->BytesRemaining() > 0) {
//   // Write() takes an IOBuffer. If it takes char*, we could
//   // simply use the regular IOBuffer like payload->data() + offset.
//   int bytes_written = Write(buf, buf->BytesRemaining());
//   buf->DidConsume(bytes_written);
// }
//
class BASE_EXPORT DrainableIOBuffer : public IOBuffer {
 public:
  DrainableIOBuffer(std::shared_ptr<IOBuffer> base, size_t size);
  ~DrainableIOBuffer() override;

  // DidConsume() changes the |data_| pointer so that |data_| always points
  // to the first unconsumed byte.
  void DidConsume(size_t bytes);

  // Returns the number of unconsumed bytes.
  size_t BytesRemaining() const;

  // Returns the number of consumed bytes.
  size_t BytesConsumed() const;

  // Seeks to an arbitrary point in the buffer. The notion of bytes consumed
  // and remaining are updated appropriately.
  void SetOffset(size_t bytes);

  size_t size() const { return size_; }

 private:
  std::shared_ptr<IOBuffer> base_;
  size_t size_;
  size_t used_;
};

// This version provides a resizable buffer and a changeable offset.
//
// GrowableIOBuffer is useful when you read data progressively without
// knowing the total size in advance. GrowableIOBuffer can be used as
// follows:
//
// buf = std::make_shared<GrowableIOBuffer>();
// buf->SetCapacity(1024);  // Initial capacity.
//
// while (!some_stream->IsEOF()) {
//   // Double the capacity if the remaining capacity is empty.
//   if (buf->RemainingCapacity() == 0)
//     buf->SetCapacity(buf->capacity() * 2);
//   int bytes_read = some_stream->Read(buf, buf->RemainingCapacity());
//   buf->set_offset(buf->offset() + bytes_read);
// }
//
class BASE_EXPORT GrowableIOBuffer : public IOBuffer {
 public:
  GrowableIOBuffer();
  ~GrowableIOBuffer() override;

  // realloc memory to the specified capacity.
  void SetCapacity(size_t capacity);
  size_t capacity() { return capacity_; }

  // |offset| moves the |data_| pointer, allowing "seeking" in the data.
  void SetOffset(size_t offset);
  size_t offset() { return offset_; }

  size_t RemainingCapacity();
  char* StartOfBuffer();

 private:
  std::unique_ptr<char, FreeDeleter> real_data_;
  size_t capacity_;
  size_t offset_;
};

// This class allows the creation of a temporary IOBuffer that doesn't really
// own the underlying buffer. Please use this class only as a last resort.
// A good example is the buffer for a synchronous operation, where we can be
// sure that nobody is keeping an extra reference to this object so the lifetime
// of the buffer can be completely managed by its intended owner.
class BASE_EXPORT WrappedIOBuffer : public IOBuffer {
 public:
  explicit WrappedIOBuffer(const char* data);
  ~WrappedIOBuffer() override;
};

}  // namespace base

#endif  // BASE_IO_BUFFER_H_
