// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 10/27/11
// Description:

#include "common/system/net/domain_resolver.h"
#include "thirdparty/gtest/gtest.h"

using namespace gdt;

TEST(DomainResolver, Query) {
  gdt::HostEntry host_entry1;
  gdt::HostEntry host_entry2;
  gdt::HostEntry host_entry3;
  int error;
  if (gdt::DomainResolver::Query("www.qq.com", &host_entry1, &error)) {
    host_entry2 = host_entry1;
    std::cout << "Query success:\n"
              << "name: " << host_entry2.Name() << "\n";

    std::cout << "Alias:";
    for (size_t i = 0; i < host_entry2.Aliases().size(); ++i)
      std::cout << " " << host_entry2.Aliases()[i];
    std::cout << "\n";

    std::cout << "IP:";
    for (size_t i = 0; i < host_entry2.Addresses().size(); ++i)
      std::cout << " " << host_entry2.Addresses()[i].ToString();
    std::cout << "\n";

    host_entry3.Swap(&host_entry2);
    EXPECT_EQ(host_entry1.Name(), host_entry3.Name());
    EXPECT_EQ(host_entry1.Aliases().size(), host_entry3.Aliases().size());
    for (size_t i = 0; i < host_entry3.Aliases().size(); ++i) {
      EXPECT_EQ(host_entry1.Aliases()[i], host_entry3.Aliases()[i]);
    }
    EXPECT_EQ(host_entry1.Addresses().size(), host_entry3.Addresses().size());
    for (size_t i = 0; i < host_entry3.Addresses().size(); ++i) {
      EXPECT_EQ(host_entry1.Addresses()[i].ToString(),
                host_entry3.Addresses()[i].ToString());
    }
    EXPECT_EQ("", host_entry2.Name());
    EXPECT_EQ(0U, host_entry2.Aliases().size());
    EXPECT_EQ(0U, host_entry2.Addresses().size());
  } else {
    std::cout << "Query error: " << gdt::DomainResolver::ErrorString(error) << '\n';
  }

  host_entry1.Clear();
  host_entry2.Clear();
}

TEST(DomainResolver, ResolveIpAddress) {
  std::vector<IPAddress> addresses;
  int error;
  if (gdt::DomainResolver::ResolveIpAddress("www.qq.com", &addresses, &error)) {
    std::cout << "Query success:\nIP:";
    for (size_t i = 0; i < addresses.size(); ++i)
      std::cout << " " << addresses[i].ToString();
    std::cout << "\n";
  } else {
    std::cout << "Query error: " << gdt::DomainResolver::ErrorString(error) << '\n';
  }
}

TEST(DomainResolver, Invalid) {
  std::vector<IPAddress> addresses;
  int error;
  EXPECT_FALSE(gdt::DomainResolver::ResolveIpAddress("non-exist.domaon",
               &addresses, &error));
  EXPECT_NE(0, error);
  EXPECT_TRUE(gdt::DomainResolver::IsFatalError(error));
  EXPECT_EQ("Unknown host", gdt::DomainResolver::ErrorString(error));
}

// } // namespace gdt