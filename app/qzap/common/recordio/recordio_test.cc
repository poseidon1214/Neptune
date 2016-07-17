// Copyright 2010, Tencent Inc.
// Author: Huican Zhu <huicanzhu@tencent.com>
//         Hangjun Ye <hansye@tencent.com>

#include <limits.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <vector>
//#include "app/qzap/common/base/compatible/stdio.h"
#include "app/qzap/common/base/scoped_ptr.h"
//#include "app/qzap/common/base/stl_container_deleter.h"
//#include "app/qzap/common/crypto/random/pseudo_random.h"
//#include "app/qzap/common/file/file.h"
#include "app/qzap/common/recordio/recordio.h"
#include "app/qzap/common/recordio/recordio_test.pb.h"
#include "app/qzap/common/utility/file_utility.h"
// includes from thirdparty
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

template <class ForwardIterator>
void STLDeleteContainerPointers(ForwardIterator begin,
                                ForwardIterator end) {
    while (begin != end) {
        ForwardIterator temp = begin;
        ++begin;
        delete *temp;
    }
}

template <class T>
void STLDeleteElements(T *container) {
    if (!container) return;
    STLDeleteContainerPointers(container->begin(), container->end());
    container->clear();
}

class PseudoRandom
{
    /// can be copied safety
public:
    explicit PseudoRandom(uint64_t seed);

    /// return random integer between 0 ~ UINT32_MAX
    uint32_t NextUInt32();

    /** Returns the next random number, limited to a given range.
        @returns a random integer between 0 (inclusive) and maxValue (exclusive).
    */
    uint32_t NextUInt32(uint32_t max_value);

    /** Returns the next random floating-point number.
        @returns a random value in the range 0 to 1.0
    */
    double NextDouble();

    /** Resets this PseudoRandom object to a given seed value. */
    void SetSeed(uint64_t seed);

    /// 生成随机字节序列
    void NextBytes(void* buffer, size_t size);
private:
    uint64_t m_seed;
};
PseudoRandom::PseudoRandom(uint64_t seed)
    : m_seed(seed)
{
}

void PseudoRandom::SetSeed(uint64_t seed)
{
    m_seed = seed;
}

uint32_t PseudoRandom::NextUInt32()
{
    m_seed = (m_seed * 0x5deece66dULL + 11) & 0xffffffffffffULL;
    return static_cast<uint32_t>(m_seed >> 16);
}

uint32_t PseudoRandom::NextUInt32(uint32_t max_value)
{
    return NextUInt32() % max_value;
}

double PseudoRandom::NextDouble()
{
    return NextUInt32() / static_cast<double>(UINT32_MAX);
}

void PseudoRandom::NextBytes(void* buffer, size_t size)
{
    unsigned char* p = reinterpret_cast<unsigned char*>(buffer);
    for (size_t i = 0; i < size; ++i)
        p[i] = NextUInt32() & UCHAR_MAX;
}
// namespace common {
namespace {
// Several internal constants from recordio.cc.
// <4 bytes magic> + <1 byte type> + <1 byte size> + <2 bytes checksum>
static const uint32_t kMinBlockHeaderSize = 8;
// <4 bytes magic> + <1 byte type> + <4 byte size> + <2 bytes checksum>
static const uint32_t kMaxBlockHeaderSize = 11;
// kMinBlockHeaderSize + <1 byte body> + <4 bytes checksum>
static const uint32_t kMinBlockSize = 13;

static const uint32_t kMaxRecordSize = 100;
// Larger than internal buffer size of RecordWriter.
static const uint32_t kLargeRecordSize = 150 * 1024;  // 150K
static const uint32_t kRecordNumber = 100 * 1024;  // 100K.
}

class RecordIOEnvironment : public testing::Environment {
public:
    virtual void SetUp() {
        //ASSERT_TRUE(File::Init());
    }
    virtual void TearDown() {
       //File::CleanUp();
    }
};

class RecordIOTest : public testing::Test {
protected:
    virtual void SetUp() {
        // We use a fixed seed to make test cases reproducible.
        m_rand.reset(new PseudoRandom(0x6d3247c9));
        m_stream.reset(new std::stringstream());
        m_file = NULL;
        m_record_reader.reset(new RecordReader(m_stream.get()));
        m_record_writer.reset(new RecordWriter(m_stream.get()));
        m_records.clear();
        m_block_offsets.clear();
        m_block_offsets.push_back(0);
        STLDeleteElements(&m_pb_records);
    }

    virtual void TearDown() {
        // Release writer before release stream, as writer dtor would call m_stream->Flush.
        m_record_writer.reset();
        m_file = NULL;
        STLDeleteElements(&m_pb_records);
    }

    int64_t StreamPosition() {
        int64_t pos = -1;
        if (m_stream.get() != NULL) {
            pos = m_stream->tellp();
        } else if (m_file != NULL) {
            pos = ftell(m_file);
        }
        // The STL implemented by VS2005 seems to have a bug: std::stringstream
        // can't be initialized by an empty string, or it would return -1 for
        // "tell{p, g}" before a character is put in the stream.
        if (pos < 0) {
            pos = 0;
        }
        return pos;
    }

    void WriteFixedSizeRecordsRandomly(uint32_t record_size,
                                       uint32_t record_number) {
        scoped_array<char> record(new char[record_size]);
        for (uint32_t i = 0; i < record_number; ++i) {
            m_rand->NextBytes(record.get(), record_size);
            WriteRecord(record.get(), record_size);
        }
    }

    void WriteVariableSizeRecordsRandomly(uint32_t max_record_size,
                                          uint32_t record_number) {
        scoped_array<char> record(new char[max_record_size]);
        for (uint32_t i = 0; i < record_number; ++i) {
            uint32_t record_size = m_rand->NextUInt32(max_record_size);
            m_rand->NextBytes(record.get(), record_size);
            WriteRecord(record.get(), record_size);
        }
    }

    void WriteRecordsInterlaced(uint32_t record_number) {
        for (uint32_t i = 0; i < record_number; ) {
            double value = m_rand->NextDouble();
            if (value < 0.50) {
                // Write a sequence of fixed size records.
                uint32_t size = m_rand->NextUInt32(kMaxRecordSize);
                uint32_t length = m_rand->NextUInt32(100);
                WriteFixedSizeRecordsRandomly(size, length);
                i += length;
            } else if (value < 0.99) {
                // Write a sequence of variable size records.
                uint32_t length = m_rand->NextUInt32(100);
                WriteVariableSizeRecordsRandomly(kMaxRecordSize, length);
                i += length;
            } else {
                // Write a large record;
                WriteFixedSizeRecordsRandomly(kLargeRecordSize, 1);
                ++i;
            }
        }
    }

    void WritePBRecords(uint32_t record_number, bool missing_required) {
        for (uint32_t i = 0; i < record_number; ++i) {
            Record* record = new Record();
            record->set_int_value(m_rand->NextUInt32());
            if (!missing_required) {
                record->set_double_value(m_rand->NextDouble());
            }
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%u", m_rand->NextUInt32());
            record->set_string_value(buffer);
            WriteMessage(record);
        }
    }

    void WritePBRecordsInterlaced(uint32_t record_number) {
        for (uint32_t i = 0; i < record_number; ) {
            uint32_t length = m_rand->NextUInt32(10);
            double value = m_rand->NextDouble();
            if (value < 0.50) {
                // Write a sequence of protocol buffer records.
                WritePBRecords(length, false);
            } else if (value < 0.75) {
                // Write a sequence of protocol buffer records missing required
                // fields.
                WritePBRecords(length, true);
            } else {
                // Write a sequence of non protocol buffer records.
                WriteRecordsInterlaced(length);
            }
            i += length;
        }
    }

    void WriteRecord(const void* data, uint32_t size) {
        const std::string record(reinterpret_cast<const char*>(data), size);
        WriteRecord(record);
    }

    void WriteRecord(const std::string& record) {
        m_records.push_back(record);
        ASSERT_TRUE(m_record_writer->WriteRecord(record.data(), record.size()));

        int64_t pos = StreamPosition();
        ASSERT_GE(pos, m_block_offsets.back());
        if (pos != m_block_offsets.back()) {
            m_block_offsets.push_back(pos);
        }
    }

    void WriteMessage(Record* record) {
        const bool retval = m_record_writer->WriteMessage(*record);
        ASSERT_EQ(record->IsInitialized(), retval);
        if (retval) {
            m_pb_records.push_back(record);
        } else {
            delete record;
        }
    }

    void Flush() {
        m_record_writer->Flush();
        int64_t pos = StreamPosition();
        ASSERT_GE(pos, m_block_offsets.back());
        if (pos != m_block_offsets.back()) {
            m_block_offsets.push_back(pos);
        }
    }

    void MessBytes(char* data, uint32_t size) {
        for (uint32_t i = 0; i < size; ++i) {
            char rand = static_cast<char>(m_rand->NextUInt32());
            // Make sure the byte is changed.
            if (rand == 0) ++rand;
            data[i] ^= rand;
        }
    }

    // Following CorruptXXX functions make a block invalid.
    void CorruptOneBlockMessBytes(std::string* block) {
        // Mess some bytes.
        uint32_t start = m_rand->NextUInt32(block->size());
        uint32_t length = m_rand->NextUInt32(block->size() - start) + 1;
        MessBytes(&block->at(start), length);
    }

    void CorruptOneBlockDeleteBytes(std::string* block) {
        // Delete some bytes.
        uint32_t start = m_rand->NextUInt32(block->size());
        uint32_t length = m_rand->NextUInt32(block->size() - start) + 1;
        block->erase(start, length);
    }

    void CorruptOneBlockAddBytes(std::string* block) {
        // Add some bytes.
        uint32_t start = m_rand->NextUInt32(block->size());
        uint32_t length = m_rand->NextUInt32(block->size()) + 1;
        block->insert(start, length, '\0');
        MessBytes(&block->at(start), length);
    }

    void CorruptOneBlockMessHeader(std::string* block) {
        // Mess the header.
        uint32_t start = m_rand->NextUInt32(kMaxBlockHeaderSize);
        uint32_t length = m_rand->NextUInt32(kMaxBlockHeaderSize - start) + 1;
        MessBytes(&block->at(start), length);
    }

    void CorruptOneBlockMessIncontinuousBytes(std::string* block) {
        // Mess some incontinuous bytes.
        uint32_t length = m_rand->NextUInt32(10) + 1;
        for (uint32_t i = 0; i < length; ++i) {
            uint32_t start = m_rand->NextUInt32(block->size());
            MessBytes(&block->at(start), 1);
        }
    }

    // Following CorruptXXX functions do NOT really make a block invalid.
    void CorruptOneBlockDuplicatePartialHeader(std::string* block) {
        // Duplicate part of header.
        uint32_t length = m_rand->NextUInt32(13) + 1;  // kMaxBlockHeaderSize;
        block->insert(0, block->substr(0, length));
    }

    void CorruptOneBlockDuplicatePartialBlock(std::string* block) {
        // Duplicate part of block.
        uint32_t start = m_rand->NextUInt32(block->size());
        uint32_t length = m_rand->NextUInt32(block->size() - start) + 1;
        block->insert(0, block->substr(start, length));
    }

    void CorruptOneBlockDuplicateBlock(std::string* block) {
        // Duplicate the whole block.
        // Make a copy for safety.
        const std::string copy = *block;
        *block += copy;
    }

    // value indicates how to corrupt a block.
    void CorruptOneBlock(const double value, std::string* block) {
        if (value < 0.20) {
            // Mess some bytes.
            CorruptOneBlockMessBytes(block);
        } else if (value < 0.40) {
            // Delete some bytes.
            CorruptOneBlockDeleteBytes(block);
        } else if (value < 0.60) {
            // Add some bytes.
            CorruptOneBlockAddBytes(block);
        } else if (value < 0.80) {
            // Mess the header.
            CorruptOneBlockMessHeader(block);
        } else {
            // Mess some incontinuous bytes.
            CorruptOneBlockMessIncontinuousBytes(block);
        }
    }

    // value indicates how to duplicate a block.
    void DuplicateOneBlock(const double value, std::string* block) {
        if (value < 0.40) {
            // Duplicate part of header.
            CorruptOneBlockDuplicatePartialHeader(block);
        } else if (value < 0.80) {
            // Duplicate part of block.
            CorruptOneBlockDuplicatePartialBlock(block);
        } else {
            // Duplicate the whole block.
            CorruptOneBlockDuplicateBlock(block);
        }
    }

    void SplitBlocksByOffsets(std::vector<std::string>* blocks) {
        const std::string content = m_stream->str();
        blocks->clear();
        for (size_t i = 0; i < m_block_offsets.size() - 1; ++i) {
            blocks->push_back(
                content.substr(
                    m_block_offsets[i],
                    m_block_offsets[i+1] - m_block_offsets[i]));
        }
    }

    // We don't re-compute the block offsets, so we can't write records anymore.
    void MergeBlocks(const std::vector<std::string>& blocks) {
        std::string content;
        for (size_t i = 0; i < blocks.size(); ++i) {
            content += blocks[i];
        }
        // Release writer before release stream, as writer dtor would call m_stream->Flush.
        m_record_writer.reset(NULL);
        m_stream.reset(
            new std::stringstream(
                content, (std::ios::in | std::ios::binary)));
        m_record_reader.reset(new RecordReader(m_stream.get()));
    }

    void CorruptBlocksRandomly() {
        std::vector<std::string> blocks;
        SplitBlocksByOffsets(&blocks);
        ASSERT_GT(blocks.size(), 0u);

        uint32_t blocks_to_corrupt = m_rand->NextUInt32(blocks.size()) + 1;
        for (uint32_t i = 0; i < blocks_to_corrupt; ++i) {
            uint32_t block_to_corrupt = m_rand->NextUInt32(blocks.size());
            double value = m_rand->NextDouble();
            CorruptOneBlock(value, &blocks[block_to_corrupt]);
        }

        MergeBlocks(blocks);
    }

    void DuplicateBlocksRandomly() {
        std::vector<std::string> blocks;
        SplitBlocksByOffsets(&blocks);
        ASSERT_GT(blocks.size(), 0u);

        uint32_t blocks_to_duplicate = m_rand->NextUInt32(blocks.size()) + 1;
        // Make sure we duplicate a whole block at least.
        uint32_t block_to_duplicate = m_rand->NextUInt32(blocks.size());
        CorruptOneBlockDuplicateBlock(&blocks[block_to_duplicate]);
        // Make sure we have some skipped bytes.
        block_to_duplicate = m_rand->NextUInt32(blocks.size());
        CorruptOneBlockDuplicatePartialHeader(&blocks[block_to_duplicate]);
        for (uint32_t i = 2; i < blocks_to_duplicate; ++i) {
            block_to_duplicate = m_rand->NextUInt32(blocks.size());
            double value = m_rand->NextDouble();
            DuplicateOneBlock(value, &blocks[block_to_duplicate]);
        }

        MergeBlocks(blocks);
    }

    void CheckRecordsExactly() {
        LOG(INFO) << "Number of block: " << m_block_offsets.size() - 1;
        //StringPiece data;
        const char* data = NULL;
        int32_t len = 0;
        uint32_t record_index = 0;
        while (m_record_reader->ReadRecord(&data, &len)) {
            if (record_index < m_records.size()) {
                //EXPECT_EQ(StringPiece(m_records[record_index]), data);
                EXPECT_EQ(m_records[record_index], std::string(data, len));
            }
            ++record_index;
        }
        EXPECT_EQ(m_records.size(), record_index)
            << "Number of read records mismatches with expectation.";
    }

    int CheckRecordsPossiblyMissed() {
        LOG(INFO) << "Number of block: " << m_block_offsets.size() - 1;
        int missed = 0;
        const char* data;
        int32_t size;
        uint32_t record_index = 0;
        while (m_record_reader->ReadRecord(&data, &size)) {
            const std::string record(data, size);
            while (record_index < m_records.size() &&
                   record != m_records[record_index]) {
                ++missed;
                ++record_index;
            }
            ++record_index;
        }
        EXPECT_LE(record_index, m_records.size())
            << "Number of read records mismatches with expectation.";
        missed += m_records.size() - record_index;
        LOG(INFO) << "Missed records: " << missed;
        LOG(INFO) << "Skipped bytes: "
                  << m_record_reader->GetAccumulatedSkippedBytes();
        return missed;
    }

    int CheckRecordsPossiblyDuplicated() {
        LOG(INFO) << "Number of block: " << m_block_offsets.size() - 1;
        int duplicated = 0;
        const char* data;
        int32_t size;
        uint32_t record_index = 0;
        while (m_record_reader->ReadRecord(&data, &size)) {
            const std::string record(data, size);
            if (record_index < m_records.size() &&
                record != m_records[record_index]) {
                ++duplicated;
                ++record_index;
                continue;
            }
            ++record_index;
        }
        EXPECT_GE(record_index, m_records.size())
            << "Number of read records mismatches with expectation.";
        LOG(INFO) << "Duplicated records: " << duplicated;
        LOG(INFO) << "Skipped bytes: "
                  << m_record_reader->GetAccumulatedSkippedBytes();
        return duplicated;
    }

    void CheckPBRecordsExactly() {
        LOG(INFO) << "Number of pb records: " << m_pb_records.size();
        Record record;
        uint32_t record_index = 0;
        while (m_record_reader->ReadMessage(&record)) {
            if (record_index < m_pb_records.size()) {
                EXPECT_EQ(m_pb_records[record_index]->int_value(),
                          record.int_value());
                EXPECT_DOUBLE_EQ(m_pb_records[record_index]->double_value(),
                                 record.double_value());
                EXPECT_EQ(m_pb_records[record_index]->string_value(),
                          record.string_value());
            }
            ++record_index;
        }
        EXPECT_EQ(m_pb_records.size(), record_index)
            << "Number of read records mismatches with expectation.";
    }

    bool LoadOldFile(const char* name) {
        FILE* file = fopen("master_file_list_old", "r");
        if (!file)
            return false;

        RecordReader reader(file);
        //StringPiece record;
        const char* record = NULL;
        int32_t len = 0;

        int success_count = 0;
        while (reader.ReadRecord(&record, &len)) {
            ++success_count;
        }
        fclose(file);
        file = NULL;
        return success_count > 0;
    }

    scoped_ptr<PseudoRandom> m_rand;
    scoped_ptr<std::stringstream> m_stream;
    FILE* m_file;
    scoped_ptr<RecordReader> m_record_reader;
    scoped_ptr<RecordWriter> m_record_writer;
    // Represents records which have been written internally, used to check with
    // the results from record reader.
    std::vector<std::string> m_records;
    std::vector<int64_t> m_block_offsets;
    // Protocol buffer records.
    std::vector<Record*> m_pb_records;
};

TEST_F(RecordIOTest, EmptyRecordIO) {
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteFixedSizeRecords0) {
    WriteFixedSizeRecordsRandomly(0, kRecordNumber);
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteFixedSizeRecords1) {
    WriteFixedSizeRecordsRandomly(1, kRecordNumber);
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteFixedSizeRecords2) {
    WriteFixedSizeRecordsRandomly(2, kRecordNumber);
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteFixedSizeRecords4) {
    WriteFixedSizeRecordsRandomly(4, kRecordNumber);
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteFixedSizeRecords8) {
    WriteFixedSizeRecordsRandomly(8, kRecordNumber);
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteFixedSizeRecords23) {
    WriteFixedSizeRecordsRandomly(23, kRecordNumber);
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteVariableSizeRecords) {
    WriteVariableSizeRecordsRandomly(kMaxRecordSize, kRecordNumber);
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteLargeRecords) {
    WriteFixedSizeRecordsRandomly(kLargeRecordSize, 10);
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteRecordsInterlaced) {
    WriteRecordsInterlaced(kRecordNumber);
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, CorruptRecords) {
    WriteRecordsInterlaced(kRecordNumber);
    Flush();
    CorruptBlocksRandomly();
    EXPECT_GT(CheckRecordsPossiblyMissed(), 0);
    EXPECT_GT(m_record_reader->GetAccumulatedSkippedBytes(), 0);
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, DuplicateRecords) {
    WriteRecordsInterlaced(kRecordNumber);
    Flush();
    DuplicateBlocksRandomly();
    EXPECT_GT(CheckRecordsPossiblyDuplicated(), 0);
    EXPECT_GT(m_record_reader->GetAccumulatedSkippedBytes(), 0);
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WritePBRecords) {
    WritePBRecords(kRecordNumber, false);
    Flush();
    CheckPBRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedRecords());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WritePBRecordsInterlaced) {
    // Write protocol buffer records and non-protocol buffer records in an
    // interlaced way.
    // Currently protocol buffer prints warnings on stderr when it fails to
    // parse, so don't write too many records here to avoid mess the screen.
    WritePBRecordsInterlaced(100);
    Flush();
    CheckPBRecordsExactly();
    // m_records records all non-protocol buffer records, so its size should be
    // equal to the number of skipped records by reader.
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(static_cast<int64_t>(m_records.size()),
        m_record_reader->GetAccumulatedSkippedRecords());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteCompressedRecordsWithNone) {
    m_record_writer.reset(new RecordWriter(
            m_stream.get(),
            RecordWriterOptions(RecordWriterOptions::DEFAULT_OPTIONS,
                                common::BlockCompressionCodec::NONE)));
    WritePBRecords(kRecordNumber, false);
    Flush();
    CheckPBRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedRecords());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}
TEST_F(RecordIOTest, WriteCompressedRecordsWithZlib) {
    m_record_writer.reset(new RecordWriter(
            m_stream.get(),
            RecordWriterOptions(RecordWriterOptions::DEFAULT_OPTIONS,
                                common::BlockCompressionCodec::ZLIB)));
    WritePBRecords(kRecordNumber, false);
    Flush();
    CheckPBRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedRecords());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteCompressedRecordsWithSnappy) {
    m_record_writer.reset(new RecordWriter(
            m_stream.get(),
            RecordWriterOptions(RecordWriterOptions::DEFAULT_OPTIONS,
                                common::BlockCompressionCodec::SNAPPY)));
    WritePBRecords(kRecordNumber, false);
    Flush();
    CheckPBRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedRecords());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteCompressedFixedSizeRecords0) {
    m_record_writer.reset(new RecordWriter(
            m_stream.get(),
            RecordWriterOptions(RecordWriterOptions::DEFAULT_OPTIONS,
                                common::BlockCompressionCodec::ZLIB)));
    WriteFixedSizeRecordsRandomly(0, kRecordNumber);
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

// Sometimes the compressed block is larger than the uncompressed one (the
// original block has been compressed, or consists of random bytes, or very
// small), the record writer outputs the uncompressed block directly, but also
// writes the RecordIOExtentHeader. The following 2 tests are to verify that the
// record reader handles it correctly.
TEST_F(RecordIOTest, WriteCompressedFixedSizeRecords1) {
    m_record_writer.reset(new RecordWriter(
            m_stream.get(),
            RecordWriterOptions(RecordWriterOptions::DEFAULT_OPTIONS,
                                common::BlockCompressionCodec::ZLIB)));
    WriteFixedSizeRecordsRandomly(1, kRecordNumber);
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteCompressedFixedSizeLargeRecords) {
    m_record_writer.reset(new RecordWriter(
            m_stream.get(),
            RecordWriterOptions(RecordWriterOptions::DEFAULT_OPTIONS,
                                common::BlockCompressionCodec::ZLIB)));
    WriteFixedSizeRecordsRandomly(kLargeRecordSize, 10);
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, WriteCompressedVariableSizeRecords) {
    m_record_writer.reset(new RecordWriter(
            m_stream.get(),
            RecordWriterOptions(RecordWriterOptions::DEFAULT_OPTIONS,
                                common::BlockCompressionCodec::ZLIB)));
    WriteVariableSizeRecordsRandomly(kMaxRecordSize, kRecordNumber);
    Flush();
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
}

TEST_F(RecordIOTest, LastIncompleteBlock) {
    WriteRecordsInterlaced(kRecordNumber);
    Flush();

    // Append some bytes from the 1st block to the end of stream.
    // We assume all blocks are "larger" than kMinBlockSize.
    const std::string content = m_stream->str();
    m_stream->clear();
    m_stream->write(content.data(), kMinBlockSize);

    CheckRecordsExactly();
    EXPECT_EQ(kMinBlockSize, m_record_reader->GetAccumulatedSkippedBytes());
    for (int i = 0; i < 10; ++i) {
        const char* data;
        int32_t size;
        EXPECT_FALSE(m_record_reader->ReadRecord(&data, &size));
    }

    // Append the remaining bytes of the whole content to the end of stream.
    m_stream->clear();
    m_stream->write(content.data() + kMinBlockSize,
                    content.size() - kMinBlockSize);

    // Some records are missed as we consider the last incomplete block as
    // corrupt.
    EXPECT_GT(CheckRecordsPossiblyMissed(), 0);
    EXPECT_GT(m_record_reader->GetAccumulatedSkippedBytes(), 0);
    for (int i = 0; i < 10; ++i) {
        const char* data;
        int32_t size;
        EXPECT_FALSE(m_record_reader->ReadRecord(&data, &size));
    }
    m_record_writer.reset();

    // Use some bytes from the 1st block as the content of stream.
    m_stream.reset(new std::stringstream(content.substr(0, kMinBlockSize)));
    m_record_reader.reset(new RecordReader(m_stream.get()));
    m_records.clear();
    m_block_offsets.clear();
    m_block_offsets.push_back(0);

    CheckRecordsExactly();
    EXPECT_EQ(kMinBlockSize, m_record_reader->GetAccumulatedSkippedBytes());
    for (int i = 0; i < 10; ++i) {
        const char* data;
        int32_t size;
        EXPECT_FALSE(m_record_reader->ReadRecord(&data, &size));
    }
}

TEST_F(RecordIOTest, ResumeLastIncompleteBlock) {
    // Make the reader resume from the last incomplete block.
    m_record_reader.reset(
        new RecordReader(
            m_stream.get(),
            RecordReaderOptions(
                RecordReaderOptions::RESUME_LAST_INCOMPLETE_BLOCK)));

    WriteRecordsInterlaced(kRecordNumber);
    Flush();

    // Append some bytes from the 1st block to the end of stream.
    // We assume all blocks are "larger" than kMinBlockSize.
    const std::string content = m_stream->str();
    m_stream->clear();
    m_stream->write(content.data(), kMinBlockSize);

    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    for (int i = 0; i < 10; ++i) {
        const char* data;
        int32_t size;
        EXPECT_FALSE(m_record_reader->ReadRecord(&data, &size));
    }

    m_record_writer.reset();
    // Append the remaining bytes of the whole content to the end of stream.
    m_stream->clear();
    m_stream->write(content.data() + kMinBlockSize,
                    content.size() - kMinBlockSize);

    // Some records are missed as we consider the last incomplete block as
    // corrupt.
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    for (int i = 0; i < 10; ++i) {
        const char* data;
        int32_t size;
        EXPECT_FALSE(m_record_reader->ReadRecord(&data, &size));
    }

    m_record_writer.reset();
    // Use some bytes from the 1st block as the content of stream.
    m_stream.reset(new std::stringstream(content.substr(0, kMinBlockSize)));
    m_record_reader.reset(
        new RecordReader(
            m_stream.get(),
            RecordReaderOptions(
                RecordReaderOptions::RESUME_LAST_INCOMPLETE_BLOCK)));
    m_records.clear();
    m_block_offsets.clear();
    m_block_offsets.push_back(0);

    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    for (int i = 0; i < 10; ++i) {
        const char* data;
        int32_t size;
        EXPECT_FALSE(m_record_reader->ReadRecord(&data, &size));
    }
}


TEST_F(RecordIOTest, WriteFixedSizeRecordsToLocalFileOwnStream) {
    std::string file_path("recordio_test_localfile_own.txt");

    m_file = fopen(file_path.c_str(), "wb");
    ASSERT_TRUE(m_file != NULL);
    m_record_writer.reset(
        new RecordWriter(
            m_file,
            RecordWriterOptions(RecordWriterOptions::OWN_STREAM)));

    // Clear m_stream must be done after reset the m_record_writer or
    // the RecordWriter will access it in the dtor.
    m_stream.reset(NULL);

    WriteFixedSizeRecordsRandomly(1, kRecordNumber);
    Flush();

    m_file = fopen(file_path.c_str(), "rb");
    ASSERT_TRUE(m_file != NULL);
    m_record_reader.reset(
        new RecordReader(
            m_file,
            RecordReaderOptions(RecordReaderOptions::OWN_STREAM)));

    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());

    //EXPECT_EQ(0, File::Remove(file_path.c_str()));
}

TEST_F(RecordIOTest, WriteFixedSizeRecordsToLocalFile) {
    std::string file_path("recordio_test_localfile.txt");
    //m_file = File::Open(file_path.c_str(), File::ENUM_FILE_OPEN_MODE_W);
    m_file = fopen(file_path.c_str(), "w");
    ASSERT_TRUE(m_file != NULL);

    m_record_writer.reset(
        new RecordWriter(m_file,
                         RecordWriterOptions(RecordWriterOptions::OWN_STREAM)));
    m_stream.reset(NULL); // clear m_stream to use m_file

    WriteFixedSizeRecordsRandomly(1, kRecordNumber);

    Flush();
    
    ASSERT_TRUE(m_file != NULL);

    //m_file = File::Open(file_path.c_str(), File::ENUM_FILE_OPEN_MODE_R);
    m_file = fopen(file_path.c_str(), "rb");
    ASSERT_TRUE(m_file != NULL);

    // reuse the same File, so Seek to begin for read
    EXPECT_EQ(0, fseek(m_file, 0, SEEK_SET));
    m_record_reader.reset(
        new RecordReader(m_file,
                         RecordReaderOptions(RecordReaderOptions::OWN_STREAM)));
    CheckRecordsExactly();
    EXPECT_EQ(0, m_record_reader->GetAccumulatedSkippedBytes());
    EXPECT_EQ(0, m_record_reader->GetUnconsumedBytes());
    //  own-stream mode, dot close file here
    //EXPECT_EQ(-1, fclose(m_file));

//    EXPECT_EQ(0, File::Remove(file_path.c_str()));
}

TEST_F(RecordIOTest, Compatible) {
    EXPECT_TRUE(LoadOldFile("master_file_list_old"));
}

TEST_F(RecordIOTest, CopyAfterWrite) {
    const std::string path1 = "read_after_write_test1.txt";
    const std::string path2 = "read_after_write_test2.txt";

    std::string file_path(path1);
    m_file = fopen(file_path.c_str(), "w");
    ASSERT_TRUE(m_file != NULL);

    m_record_writer.reset(
        new RecordWriter(m_file,
                         RecordWriterOptions(RecordWriterOptions::OWN_STREAM)));
    m_stream.reset(NULL); // clear m_stream to use m_file

    WriteFixedSizeRecordsRandomly(1, kRecordNumber);
    Flush();

    // Copy after flush, the two file must be the same.
    ASSERT_EQ(0, system(std::string(" cp " + path1 + " " + path2).c_str()));

    m_record_writer.reset(NULL);
    // Verify two files are the same.
    std::string content1;
    std::string content2;
    ReadFileToString(path1, &content1);
    ReadFileToString(path2, &content2);
    EXPECT_EQ(content1, content2);
}

// Regression for a bug: WriteRecord calls FlushFromExternalBuffer when buffer too large,
// but did not call underlying stream's flush.
// In RecordWriter.Flush, it also does not flush when m_records==0.
// To fix it, we always call underlying stream's flush in Flush.
TEST_F(RecordIOTest, EnsureFlushData) {
    const std::string old_file_name = "test_data.pb";
    const std::string new_file_name = "test_data.pb.new";

    FILE* old_file = fopen(old_file_name.c_str(), "r");
    FILE* new_file = fopen(new_file_name.c_str(), "w");
    ASSERT_TRUE(old_file != NULL);
    ASSERT_TRUE(new_file != NULL);

    RecordReader reader(old_file);
    RecordWriter writer(new_file);

    UpdaterRecord record;
    int32_t count = 0;
    while (reader.ReadMessage(&record)) {
        EXPECT_TRUE(writer.WriteMessage(record));
        ++count;
    }
    EXPECT_GT(count, 0);
    EXPECT_TRUE(writer.Flush());

    // Verify the two files are the same.
    std::string content1;
    std::string content2;
    ReadFileToString(old_file_name, &content1);
    ReadFileToString(new_file_name, &content2);

    ASSERT_GT(content2.size(), 0u);
    EXPECT_TRUE(content1 == content2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // Must after InitGoogleTest to avoid remove flags before Init to hide gtest_filter etc.
    google::AllowCommandLineReparsing();
    CHECK(google::ParseCommandLineFlags(&argc, &argv, true))
        << "fail to parse arguments.";
    google::InitGoogleLogging(argv[0]);

    testing::AddGlobalTestEnvironment(new RecordIOEnvironment);

    return RUN_ALL_TESTS();
}

// } // namespace common
