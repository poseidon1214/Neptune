// Copyright 2010, Tencent Inc.
// Author: Huican Zhu <huicanzhu@tencent.com>
//         Hangjun Ye <hansye@tencent.com>

#include "app/qzap/common/recordio/recordio_jna.h"
#include <fstream>
#include <new>
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/recordio/recordio.h"
#include "thirdparty/glog/logging.h"

// includes from thirdparty

// namespace common {


RecordReader* OpenRecordReader(const char* filename, uint32_t options) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }
    RecordReader* record_reader =
        new (std::nothrow) RecordReader(
            file, RecordReaderOptions(options | RecordReaderOptions::OWN_STREAM));
    return record_reader;
}

RecordReader* OpenSplitRecordReader(const char* filename, uint32_t options,
                                    int64_t start, int64_t end) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }
    RecordReader* record_reader =
        new (std::nothrow) RecordReader(
            file, RecordReaderOptions(options | RecordReaderOptions::OWN_STREAM,
                                      start, end));
    return record_reader;
}

bool ReadRecord(RecordReader* record_reader,
                const char** data,
                int32_t* size) {
    CHECK_NOTNULL(record_reader);
    return record_reader->ReadRecord(data, size);
}

void CloseRecordReader(RecordReader* record_reader) {
    CHECK_NOTNULL(record_reader);
    delete record_reader;
}

RecordWriter* OpenRecordWriter(const char* filename,
                               uint32_t options,
                               uint32_t compression_codec) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        return NULL;
    }

    RecordWriter* record_writer =
        new (std::nothrow) RecordWriter(
            file,
            RecordWriterOptions(options | RecordWriterOptions::OWN_STREAM,
                                compression_codec));
    return record_writer;
}

bool WriteRecord(RecordWriter* record_writer, const char* data, int32_t size) {
    CHECK_NOTNULL(record_writer);
    return record_writer->WriteRecord(data, size);
}

bool FlushRecordWriter(RecordWriter* record_writer) {
    CHECK_NOTNULL(record_writer);
    return record_writer->Flush();
}

void CloseRecordWriter(RecordWriter* record_writer) {
    CHECK_NOTNULL(record_writer);
    delete record_writer;
}

// } // namespace common
