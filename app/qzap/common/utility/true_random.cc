// Copyright (c) 2010, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>

#include "app/qzap/common/utility/true_random.h"

#include <fcntl.h>
#include <unistd.h>

#include <limits.h>
#include <stdlib.h>

namespace gdt {

TrueRandom::TrueRandom()
{
    m_fd = open("/dev/urandom", O_RDONLY, 0);
    if (m_fd < 0)
    {
        abort();
    }
}

TrueRandom::~TrueRandom()
{
    close(m_fd);
    m_fd = -1;
}

bool TrueRandom::NextBytes(void* buffer, size_t size)
{
    return read(m_fd, buffer, size) == static_cast<int>(size);
}

uint32_t TrueRandom::NextUInt32()
{
    uint32_t bytes = -1;
    NextBytes(&bytes, sizeof(bytes));
    return bytes;
}

uint32_t TrueRandom::NextUInt32(uint32_t max_value)
{
    return NextUInt32() % max_value;
}

double TrueRandom::NextDouble()
{
    uint32_t n = NextUInt32();
    return static_cast<double>(n) / UINT32_MAX;
}

}  // namespace gdt

