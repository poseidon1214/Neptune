// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-3-28
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/sstable/sstable.h"
#include "app/qzap/common/utility/file_utility.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/leveldb/env.h"

DEFINE_string(file, "", "");

int main(int argc, char** argv) {
  ::google::ParseCommandLineFlags(&argc, &argv, false);
  ::google::InitGoogleLogging(argv[0]);
  scoped_ptr<SSTable> table(SSTable::OpenFromHDFSFile(FLAGS_file));
  CHECK(table != NULL);
  scoped_ptr<SSTable::Iterator> it(table->NewIterator());
  CHECK(it != NULL);
  CHECK(! it->Valid());
  it->SeekToFirst();
  while (it->Valid()) {
    LOG(INFO) << "key: " << it->key() << " value size: " << it->value().size();
    it->Next();
  }
}
