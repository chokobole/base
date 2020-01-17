// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/socket/address_list.h"

#include "base/socket/ip_address.h"
#include "base/socket/sockaddr_storage.h"
#include "base/socket/sys_addrinfo.h"
#include "base/stl_util.h"
#include "base/strings/string_util.h"
#include "base/sys_byteorder.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAre;

namespace base {
namespace {

TEST(AddressListTest, CreateFromAddrinfo) {
  // Create an 4-element addrinfo.
  const unsigned kNumElements = 4;
  SockaddrStorage storage[kNumElements];
  struct addrinfo ai[kNumElements];
  for (unsigned i = 0; i < kNumElements; ++i) {
    struct sockaddr_in* addr =
        reinterpret_cast<struct sockaddr_in*>(storage[i].addr);
    storage[i].addr_len = sizeof(struct sockaddr_in);
    // Populating the address with { i, i, i, i }.
    memset(&addr->sin_addr, i, IPAddress::kIPv4AddressSize);
    addr->sin_family = AF_INET;
    // Set port to i << 2;
    addr->sin_port = HostToNet16(static_cast<uint16_t>(i << 2));
    memset(&ai[i], 0x0, sizeof(ai[i]));
    ai[i].ai_family = addr->sin_family;
    ai[i].ai_socktype = SOCK_STREAM;
    ai[i].ai_addrlen = storage[i].addr_len;
    ai[i].ai_addr = storage[i].addr;
    if (i + 1 < kNumElements) ai[i].ai_next = &ai[i + 1];
  }

  AddressList list = AddressList::CreateFromAddrinfo(&ai[0]);

  ASSERT_EQ(kNumElements, list.size());
  for (size_t i = 0; i < list.size(); ++i) {
    EXPECT_EQ(ADDRESS_FAMILY_IPV4, list[i].GetFamily());
    // Only check the first byte of the address.
    EXPECT_EQ(i, list[i].address().bytes()[0]);
    EXPECT_EQ(static_cast<int>(i << 2), list[i].port());
  }

  // Check if operator= works.
  AddressList copy;
  copy = list;
  ASSERT_EQ(kNumElements, copy.size());

  // Check if copy is independent.
  copy[1] = IPEndPoint(copy[2].address(), 0xBEEF);
  // Original should be unchanged.
  EXPECT_EQ(1u, list[1].address().bytes()[0]);
  EXPECT_EQ(1 << 2, list[1].port());
}

TEST(AddressListTest, CreateFromIPAddressList) {
  struct TestData {
    std::string ip_address;
    const char* in_addr;
    int ai_family;
    size_t ai_addrlen;
    size_t in_addr_offset;
    size_t in_addr_size;
  } tests[] = {
      {
          "127.0.0.1",
          "\x7f\x00\x00\x01",
          AF_INET,
          sizeof(struct sockaddr_in),
          offsetof(struct sockaddr_in, sin_addr),
          sizeof(struct in_addr),
      },
      {
          "2001:db8:0::42",
          "\x20\x01\x0d\xb8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x42",
          AF_INET6,
          sizeof(struct sockaddr_in6),
          offsetof(struct sockaddr_in6, sin6_addr),
          sizeof(struct in6_addr),
      },
      {
          "192.168.1.1",
          "\xc0\xa8\x01\x01",
          AF_INET,
          sizeof(struct sockaddr_in),
          offsetof(struct sockaddr_in, sin_addr),
          sizeof(struct in_addr),
      },
  };

  // Construct a list of ip addresses.
  IPAddressList ip_list;
  for (const auto& test : tests) {
    IPAddress ip_address;
    ASSERT_TRUE(ip_address.AssignFromIPLiteral(test.ip_address));
    ip_list.push_back(ip_address);
  }

  AddressList test_list = AddressList::CreateFromIPAddressList(ip_list);
  EXPECT_EQ(size(tests), test_list.size());
}

TEST(AddressListTest, DeduplicatesEmptyAddressList) {
  AddressList empty;
  empty.Deduplicate();
  EXPECT_EQ(empty.size(), 0u);
}

TEST(AddressListTest, DeduplicatesSingletonAddressList) {
  AddressList singleton;
  singleton.push_back(IPEndPoint());
  singleton.Deduplicate();
  EXPECT_THAT(singleton, ElementsAre(IPEndPoint()));
}

TEST(AddressListTest, DeduplicatesLongerAddressList) {
  AddressList several;
  several = {IPEndPoint(IPAddress(0, 0, 0, 1), 0),
             IPEndPoint(IPAddress(0, 0, 0, 2), 0),
             IPEndPoint(IPAddress(0, 0, 0, 2), 0),
             IPEndPoint(IPAddress(0, 0, 0, 3), 0),
             IPEndPoint(IPAddress(0, 0, 0, 2), 0),
             IPEndPoint(IPAddress(0, 0, 0, 1), 0),
             IPEndPoint(IPAddress(0, 0, 0, 2), 0),
             IPEndPoint(IPAddress(0, 0, 0, 3), 0),
             IPEndPoint(IPAddress(0, 0, 0, 2), 0)};
  several.Deduplicate();

  // Deduplication should preserve the order of the first instances
  // of the unique addresses.
  EXPECT_THAT(several, ElementsAre(IPEndPoint(IPAddress(0, 0, 0, 1), 0),
                                   IPEndPoint(IPAddress(0, 0, 0, 2), 0),
                                   IPEndPoint(IPAddress(0, 0, 0, 3), 0)));
}

}  // namespace
}  // namespace base
