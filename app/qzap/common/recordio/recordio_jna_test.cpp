// Copyright (c) 2011, Tencent Inc. All rights reserved.

#include "app/qzap/common/recordio/recordio_jna.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/gtest/gtest.h"

// namespace common {

class RecordIoJnaTest : public testing::Test
{
protected:
    static const char* kTestFileName;
    static const int M = 100;
    static const int N = 100;

protected:
    virtual void SetUp()
    {
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                test_data[i][j] = static_cast<char>((i * j) / 128);
            }
        }
    }

    virtual void TearDown()
    {
        //::unlink(kTestFileName);
    }

protected:
    char test_data[M][N];
};

const char* RecordIoJnaTest::kTestFileName = "recordio_jna.recordio";
const int RecordIoJnaTest::M;
const int RecordIoJnaTest::N;

TEST_F(RecordIoJnaTest, Test)
{
    {
        RecordWriter *writer = OpenRecordWriter(kTestFileName, 0);
        EXPECT_TRUE(writer != NULL);
        for (int i = 0; i < M; ++i) {
            EXPECT_TRUE(WriteRecord(writer, test_data[i], N));
        }
        FlushRecordWriter(writer);
        CloseRecordWriter(writer);
    }

    {
        RecordReader *reader = OpenSplitRecordReader(kTestFileName, 0, 0, 0);
        EXPECT_TRUE(reader != NULL);
        const char* data;
        int len = 0;
        while ((ReadRecord(reader, &data, &len)) != false) {
            EXPECT_EQ(N, len);
        }
        CloseRecordReader(reader);
    }
}

// } // namespace common
