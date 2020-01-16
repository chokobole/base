// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/socket/ip_address.h"

#include <stdlib.h>

#include <algorithm>
#include <climits>

#include "absl/strings/ascii.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/substitute.h"
#include "base/containers/stack_container.h"
#include "base/stl_util.h"
#include "base/strings/string_number_conversions.h"

namespace base {
namespace {

struct Range {
  size_t start;
  size_t length;

  Range() { Reset(); }

  Range(size_t start, size_t length) : start(start), length(length) {}

  bool IsValid() const { return start != std::numeric_limits<size_t>::max(); }

  void Reset() {
    start = std::numeric_limits<size_t>::max();
    length = 0;
  }

  size_t end() { return start + length; }
};

// The prefix for IPv6 mapped IPv4 addresses.
// https://tools.ietf.org/html/rfc4291#section-2.5.5.2
constexpr uint8_t kIPv4MappedPrefix[] = {0, 0, 0, 0, 0,    0,
                                         0, 0, 0, 0, 0xFF, 0xFF};

// Note that this function assumes:
// * |ip_address| is at least |prefix_length_in_bits| (bits) long;
// * |ip_prefix| is at least |prefix_length_in_bits| (bits) long.
bool IPAddressPrefixCheck(const IPAddressBytes& ip_address,
                          const uint8_t* ip_prefix,
                          size_t prefix_length_in_bits) {
  // Compare all the bytes that fall entirely within the prefix.
  size_t num_entire_bytes_in_prefix = prefix_length_in_bits / 8;
  for (size_t i = 0; i < num_entire_bytes_in_prefix; ++i) {
    if (ip_address[i] != ip_prefix[i]) return false;
  }

  // In case the prefix was not a multiple of 8, there will be 1 byte
  // which is only partially masked.
  size_t remaining_bits = prefix_length_in_bits % 8;
  if (remaining_bits != 0) {
    uint8_t mask = 0xFF << (8 - remaining_bits);
    size_t i = num_entire_bytes_in_prefix;
    if ((ip_address[i] & mask) != (ip_prefix[i] & mask)) return false;
  }
  return true;
}

// Returns false if |ip_address| matches any of the reserved IPv4 ranges. This
// method operates on a list of reserved IPv4 ranges. Some ranges are
// consolidated.
// Sources for info:
// www.iana.org/assignments/ipv4-address-space/ipv4-address-space.xhtml
// www.iana.org/assignments/iana-ipv4-special-registry/
// iana-ipv4-special-registry.xhtml
bool IsPubliclyRoutableIPv4(const IPAddressBytes& ip_address) {
  // Different IP versions have different range reservations.
  DCHECK_EQ(IPAddress::kIPv4AddressSize, ip_address.size());
  struct {
    const uint8_t address[4];
    size_t prefix_length_in_bits;
  } static const kReservedIPv4Ranges[] = {
      {{0, 0, 0, 0}, 8},     {{10, 0, 0, 0}, 8},      {{100, 64, 0, 0}, 10},
      {{127, 0, 0, 0}, 8},   {{169, 254, 0, 0}, 16},  {{172, 16, 0, 0}, 12},
      {{192, 0, 2, 0}, 24},  {{192, 88, 99, 0}, 24},  {{192, 168, 0, 0}, 16},
      {{198, 18, 0, 0}, 15}, {{198, 51, 100, 0}, 24}, {{203, 0, 113, 0}, 24},
      {{224, 0, 0, 0}, 3}};

  for (const auto& range : kReservedIPv4Ranges) {
    if (IPAddressPrefixCheck(ip_address, range.address,
                             range.prefix_length_in_bits)) {
      return false;
    }
  }

  return true;
}

// Returns false if |ip_address| matches any of the IPv6 ranges IANA reserved
// for local networks. This method operates on an allowlist of non-reserved
// IPv6 ranges, plus the list of reserved IPv4 ranges mapped to IPv6.
// Sources for info:
// www.iana.org/assignments/ipv6-address-space/ipv6-address-space.xhtml
bool IsPubliclyRoutableIPv6(const IPAddressBytes& ip_address) {
  DCHECK_EQ(IPAddress::kIPv6AddressSize, ip_address.size());
  struct {
    const uint8_t address_prefix[2];
    size_t prefix_length_in_bits;
  } static const kPublicIPv6Ranges[] = {// 2000::/3  -- Global Unicast
                                        {{0x20, 0}, 3},
                                        // ff00::/8  -- Multicast
                                        {{0xff, 0}, 8}};

  for (const auto& range : kPublicIPv6Ranges) {
    if (IPAddressPrefixCheck(ip_address, range.address_prefix,
                             range.prefix_length_in_bits)) {
      return true;
    }
  }

  IPAddress addr(ip_address);
  if (addr.IsIPv4MappedIPv6()) {
    IPAddress ipv4 = ConvertIPv4MappedIPv6ToIPv4(addr);
    return IsPubliclyRoutableIPv4(ipv4.bytes());
  }

  return false;
}

bool DoParseIPv4LiteralToBytes(absl::string_view ip_literal,
                               IPAddressBytes* bytes) {
  bytes->Resize(0);
  size_t cur = 0;
  for (int i = 0; i < 4; ++i) {
    uint16_t num = 0;
    size_t num_count = 0;
    while (cur < ip_literal.length()) {
      char c = ip_literal[cur++];
      if (c == '.') {
        break;
      } else if (absl::ascii_isdigit(c)) {
        num = num * 10 + (c - '0');
        num_count++;
        if (num_count > 3) return false;
      } else {
        return false;
      }
    }
    if (num_count == 0)
      return false;
    else if (num < 256) {
      bytes->push_back(static_cast<uint8_t>(num));
    } else {
      return false;
    }
  }
  return cur == ip_literal.length();
}

bool DoParseIPv6LiteralToBytes(absl::string_view ip_literal,
                               IPAddressBytes* bytes) {
  StackVector<uint8_t, 16> nums;
  size_t double_colon_idx = std::numeric_limits<size_t>::max();
  size_t cur = 0;
  while (cur < ip_literal.length()) {
    uint32_t num = 0;
    size_t num_count = 0;
    bool met_double_colon = false;
    bool met_ipv4_address = false;
    size_t maybe_ipv4_idx = cur;
    while (cur < ip_literal.length()) {
      char c = ip_literal[cur++];
      if (c == ':') {
        if (double_colon_idx == std::numeric_limits<size_t>::max() &&
            cur < ip_literal.length() && ':' == ip_literal[cur]) {
          met_double_colon = true;
          cur++;
        }
        break;
      } else if (absl::ascii_isxdigit(c)) {
        if (absl::ascii_isdigit(c)) {
          num = num * 16 + (c - '0');
        } else if (absl::ascii_islower(c)) {
          num = num * 16 + 10 + (c - 'a');
        } else {
          num = num * 16 + 10 + (c - 'A');
        }
        num_count++;
        if (num_count > 4) return false;
      } else if (c == '.') {
        met_ipv4_address = true;
        break;
      } else {
        return false;
      }
    }
    if (num_count == 0) {
      // starts with ::
      if (!met_double_colon) return false;
    } else if (met_ipv4_address) {
      ip_literal.remove_prefix(maybe_ipv4_idx);
      IPAddressBytes ipv4_bytes;
      if (!DoParseIPv4LiteralToBytes(ip_literal, &ipv4_bytes)) return false;
      for (int i = 0; i < 4; ++i) {
        nums->push_back(ipv4_bytes[i]);
      }
      cur = ip_literal.length();
      break;
    } else if (num < 65536) {
      // <number>::
      nums->push_back(static_cast<uint8_t>(num / 256));
      nums->push_back(static_cast<uint8_t>(num % 256));
    } else {
      return false;
    }

    if (met_double_colon) {
      double_colon_idx = nums->size();
    }
  }
  bytes->Resize(16);  // 128 bits.
  if (double_colon_idx == std::numeric_limits<size_t>::max()) {
    if (nums->size() != 16) return false;
    std::copy_n(nums->data(), nums->size(), bytes->data());
  } else {
    std::copy_n(nums->data(), double_colon_idx, bytes->data());
    for (size_t i = 0; i < 16 - nums->size(); ++i) {
      (*bytes)[double_colon_idx + i] = 0;
    }
    std::copy_n(nums->data() + double_colon_idx,
                nums->size() - double_colon_idx,
                bytes->data() + (16 - nums->size() + double_colon_idx));
  }

  return cur == ip_literal.length();
}

bool ParseIPLiteralToBytes(absl::string_view ip_literal,
                           IPAddressBytes* bytes) {
  // |ip_literal| could be either an IPv4 or an IPv6 literal. If it contains
  // a colon however, it must be an IPv6 address.
  if (ip_literal.find(':') != std::string::npos) {
    return DoParseIPv6LiteralToBytes(ip_literal, bytes);
  }

  // Otherwise the string is an IPv4 address.
  return DoParseIPv4LiteralToBytes(ip_literal, bytes);
}

Range FindLongestZeros(const uint8_t address[16]) {
  Range longest_zeros;
  Range current_zeros;

  for (int i = 0; i < 16; i += 2) {
    bool is_zero = address[i] == 0 && address[i + 1] == 0;
    if (is_zero) {
      if (!current_zeros.IsValid()) {
        current_zeros = Range(i, 0);
      }
      current_zeros.length += 2;
    }

    if (!is_zero || i == 14) {
      if (current_zeros.length > 2 &&
          current_zeros.length > longest_zeros.length) {
        longest_zeros = current_zeros;
      }
      current_zeros.Reset();
    }
  }

  return longest_zeros;
}

std::string IPv4ToString(const uint8_t address[4]) {
  return absl::StrCat(address[0], ".", address[1], ".", address[2], ".",
                      address[3]);
}

std::string IPv6ToString(const uint8_t address[16]) {
  // http://tools.ietf.org/html/draft-kawamura-ipv6-text-representation-01#section-4
  std::string output;
  Range longest_zeros = FindLongestZeros(address);

  for (int i = 0; i <= 14;) {
    // We check 2 bytes at a time, from bytes (0, 1) to (14, 15), inclusive.
    DCHECK(i % 2 == 0);
    if (i == longest_zeros.start && longest_zeros.length > 0) {
      // Jump over the contraction.
      if (i == 0) output.push_back(':');
      output.push_back(':');
      i = longest_zeros.end();
    } else {
      // Consume the next 16 bits from |address|.
      int x = address[i] << 8 | address[i + 1];

      i += 2;

      // Stringify the 16 bit number (at most requires 4 hex digits).
      absl::StrAppend(&output, absl::Hex(x));

      // Put a colon after each number, except the last.
      if (i < 16) output.push_back(':');
    }
  }
  return output;
}

}  // namespace

IPAddressBytes::IPAddressBytes() : size_(0) {}

IPAddressBytes::IPAddressBytes(const uint8_t* data, size_t data_len) {
  Assign(data, data_len);
}

IPAddressBytes::~IPAddressBytes() = default;
IPAddressBytes::IPAddressBytes(IPAddressBytes const& other) = default;

void IPAddressBytes::Assign(const uint8_t* data, size_t data_len) {
  size_ = data_len;
  CHECK_GE(16u, data_len);
  std::copy_n(data, data_len, bytes_.data());
}

bool IPAddressBytes::operator<(const IPAddressBytes& other) const {
  if (size_ == other.size_)
    return std::lexicographical_compare(begin(), end(), other.begin(),
                                        other.end());
  return size_ < other.size_;
}

bool IPAddressBytes::operator==(const IPAddressBytes& other) const {
  return size_ == other.size_ && std::equal(begin(), end(), other.begin());
}

bool IPAddressBytes::operator!=(const IPAddressBytes& other) const {
  return !(*this == other);
}

IPAddress::IPAddress() = default;

IPAddress::IPAddress(const IPAddress& other) = default;

IPAddress::IPAddress(const IPAddressBytes& address) : ip_address_(address) {}

IPAddress::IPAddress(const uint8_t* address, size_t address_len)
    : ip_address_(address, address_len) {}

IPAddress::IPAddress(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) {
  ip_address_.push_back(b0);
  ip_address_.push_back(b1);
  ip_address_.push_back(b2);
  ip_address_.push_back(b3);
}

IPAddress::IPAddress(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4,
                     uint8_t b5, uint8_t b6, uint8_t b7, uint8_t b8, uint8_t b9,
                     uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13,
                     uint8_t b14, uint8_t b15) {
  ip_address_.push_back(b0);
  ip_address_.push_back(b1);
  ip_address_.push_back(b2);
  ip_address_.push_back(b3);
  ip_address_.push_back(b4);
  ip_address_.push_back(b5);
  ip_address_.push_back(b6);
  ip_address_.push_back(b7);
  ip_address_.push_back(b8);
  ip_address_.push_back(b9);
  ip_address_.push_back(b10);
  ip_address_.push_back(b11);
  ip_address_.push_back(b12);
  ip_address_.push_back(b13);
  ip_address_.push_back(b14);
  ip_address_.push_back(b15);
}

IPAddress::~IPAddress() = default;

bool IPAddress::IsIPv4() const {
  return ip_address_.size() == kIPv4AddressSize;
}

bool IPAddress::IsIPv6() const {
  return ip_address_.size() == kIPv6AddressSize;
}

bool IPAddress::IsValid() const { return IsIPv4() || IsIPv6(); }

bool IPAddress::IsPubliclyRoutable() const {
  if (IsIPv4()) {
    return IsPubliclyRoutableIPv4(ip_address_);
  } else if (IsIPv6()) {
    return IsPubliclyRoutableIPv6(ip_address_);
  }
  return true;
}

bool IPAddress::IsZero() const {
  for (auto x : ip_address_) {
    if (x != 0) return false;
  }

  return !empty();
}

bool IPAddress::IsIPv4MappedIPv6() const {
  return IsIPv6() && IPAddressStartsWith(*this, kIPv4MappedPrefix);
}

bool IPAddress::IsLoopback() const {
  // 127.0.0.1/8
  if (IsIPv4()) return ip_address_[0] == 127;

  // ::1
  if (IsIPv6()) {
    for (size_t i = 0; i + 1 < ip_address_.size(); ++i) {
      if (ip_address_[i] != 0) return false;
    }
    return ip_address_.back() == 1;
  }

  return false;
}

bool IPAddress::IsLinkLocal() const {
  // 169.254.0.0/16
  if (IsIPv4()) return (ip_address_[0] == 169) && (ip_address_[1] == 254);

  // [::ffff:169.254.0.0]/112
  if (IsIPv4MappedIPv6())
    return (ip_address_[12] == 169) && (ip_address_[13] == 254);

  // [fe80::]/10
  if (IsIPv6())
    return (ip_address_[0] == 0xFE) && ((ip_address_[1] & 0xC0) == 0x80);

  return false;
}

bool IPAddress::AssignFromIPLiteral(absl::string_view ip_literal) {
  bool success = ParseIPLiteralToBytes(ip_literal, &ip_address_);
  if (!success) ip_address_.Resize(0);
  return success;
}

std::vector<uint8_t> IPAddress::CopyBytesToVector() const {
  return std::vector<uint8_t>(ip_address_.begin(), ip_address_.end());
}

// static
IPAddress IPAddress::IPv4Localhost() {
  static const uint8_t kLocalhostIPv4[] = {127, 0, 0, 1};
  return IPAddress(kLocalhostIPv4);
}

// static
IPAddress IPAddress::IPv6Localhost() {
  static const uint8_t kLocalhostIPv6[] = {0, 0, 0, 0, 0, 0, 0, 0,
                                           0, 0, 0, 0, 0, 0, 0, 1};
  return IPAddress(kLocalhostIPv6);
}

// static
IPAddress IPAddress::AllZeros(size_t num_zero_bytes) {
  CHECK_LE(num_zero_bytes, 16u);
  IPAddress result;
  for (size_t i = 0; i < num_zero_bytes; ++i) result.ip_address_.push_back(0u);
  return result;
}

// static
IPAddress IPAddress::IPv4AllZeros() { return AllZeros(kIPv4AddressSize); }

// static
IPAddress IPAddress::IPv6AllZeros() { return AllZeros(kIPv6AddressSize); }

bool IPAddress::operator==(const IPAddress& that) const {
  return ip_address_ == that.ip_address_;
}

bool IPAddress::operator!=(const IPAddress& that) const {
  return ip_address_ != that.ip_address_;
}

bool IPAddress::operator<(const IPAddress& that) const {
  // Sort IPv4 before IPv6.
  if (ip_address_.size() != that.ip_address_.size()) {
    return ip_address_.size() < that.ip_address_.size();
  }

  return ip_address_ < that.ip_address_;
}

std::string IPAddress::ToString() const {
  if (IsIPv4()) {
    return IPv4ToString(ip_address_.data());
  } else if (IsIPv6()) {
    return IPv6ToString(ip_address_.data());
  }

  return "";
}

std::string IPAddressToStringWithPort(const IPAddress& address, uint16_t port) {
  std::string address_str = address.ToString();
  if (address_str.empty()) return address_str;

  if (address.IsIPv6()) {
    // Need to bracket IPv6 addresses since they contain colons.
    return absl::Substitute("[$0]:$1", address_str, port);
  }
  return absl::Substitute("$0:$1", address_str, port);
}

std::string IPAddressToPackedString(const IPAddress& address) {
  return std::string(reinterpret_cast<const char*>(address.bytes().data()),
                     address.size());
}

IPAddress ConvertIPv4ToIPv4MappedIPv6(const IPAddress& address) {
  DCHECK(address.IsIPv4());
  // IPv4-mapped addresses are formed by:
  // <80 bits of zeros>  + <16 bits of ones> + <32-bit IPv4 address>.
  StackVector<uint8_t, 16> bytes;
  bytes->insert(bytes->end(), std::begin(kIPv4MappedPrefix),
                std::end(kIPv4MappedPrefix));
  bytes->insert(bytes->end(), address.bytes().begin(), address.bytes().end());
  return IPAddress(bytes->data(), bytes->size());
}

IPAddress ConvertIPv4MappedIPv6ToIPv4(const IPAddress& address) {
  DCHECK(address.IsIPv4MappedIPv6());

  StackVector<uint8_t, 16> bytes;
  bytes->insert(bytes->end(), address.bytes().begin() + size(kIPv4MappedPrefix),
                address.bytes().end());
  return IPAddress(bytes->data(), bytes->size());
}

bool IPAddressMatchesPrefix(const IPAddress& ip_address,
                            const IPAddress& ip_prefix,
                            size_t prefix_length_in_bits) {
  // Both the input IP address and the prefix IP address should be either IPv4
  // or IPv6.
  DCHECK(ip_address.IsValid());
  DCHECK(ip_prefix.IsValid());

  DCHECK_LE(prefix_length_in_bits, ip_prefix.size() * 8);

  // In case we have an IPv6 / IPv4 mismatch, convert the IPv4 addresses to
  // IPv6 addresses in order to do the comparison.
  if (ip_address.size() != ip_prefix.size()) {
    if (ip_address.IsIPv4()) {
      return IPAddressMatchesPrefix(ConvertIPv4ToIPv4MappedIPv6(ip_address),
                                    ip_prefix, prefix_length_in_bits);
    }
    return IPAddressMatchesPrefix(ip_address,
                                  ConvertIPv4ToIPv4MappedIPv6(ip_prefix),
                                  96 + prefix_length_in_bits);
  }

  return IPAddressPrefixCheck(ip_address.bytes(), ip_prefix.bytes().data(),
                              prefix_length_in_bits);
}

}  // namespace base
