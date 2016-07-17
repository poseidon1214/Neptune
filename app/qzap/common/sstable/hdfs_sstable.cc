#include "thirdparty/glog/logging.h"
#include "app/qzap/common/sstable/sstable.h"
#include "app/qzap/common/utility/file_utility.h"
#include "app/qzap/common/base/string_utility.h"
#include "thirdparty/leveldb/env.h"
#include "thirdparty/leveldb/options.h"
#include "thirdparty/leveldb/table.h"
#include "thirdparty/leveldb/table_builder.h"
#include "thirdparty/libhdfs/hdfs.h"

namespace leveldb {

class HDFSRandomAccessFile : public RandomAccessFile {
  private:
    std::string file_name_;
    hdfsFS fs_;
    hdfsFile file_;
    size_t size_;
  public:
    HDFSRandomAccessFile() : fs_(NULL), file_(NULL), size_(0){
    }
    size_t size() const {
      return size_;
    }
    ~HDFSRandomAccessFile() {
      if (fs_ != NULL) {
        if (file_ != NULL && hdfsCloseFile(fs_, file_) != 0) {
          LOG(ERROR) << "Fail to close hdfs: " << file_name_;
        }
        else {
          file_ = NULL;
        }
        hdfsDisconnect(fs_);
        fs_ = NULL;
      }
    }
    static HDFSRandomAccessFile *Open(const std::string &file_name) {
      std::vector<std::string> seg;
      if (!SplitString(file_name, "/", &seg) && seg.size() < 3u) {
        LOG(ERROR) << "Invalid format" << file_name;
        return NULL;
      }
      std::vector<std::string> port_seg;
      if (!SplitString(seg[2], ":", &port_seg) && seg.size() != 2u) {
        LOG(ERROR) << "Invalid format" << file_name;
        return NULL;
      }

      std::string host = port_seg[0];
      uint32_t port = 0;
      if (!StringToNumeric(port_seg[1], &port)) {
        LOG(ERROR) << "Fail to get port " << file_name;
        return NULL;
      }
      VLOG(1) << "connect to host " << host << " port " << port ;
      hdfsFS fs = hdfsConnectNewInstance(host.c_str(), port);
      if (!fs) {
        LOG(ERROR) << "Fail to open hdfs: " << file_name;
        return NULL;
      }
      hdfsFile file = hdfsOpenFile(fs, file_name.c_str(), O_RDONLY, 0, 0, 0);
      if (!file) {
        LOG(ERROR) << "Fail to open file: " << file_name;
        hdfsDisconnect(fs);
        return NULL;
      }
      hdfsFileInfo *file_info = hdfsGetPathInfo(fs, file_name.c_str());
      if (file_info == NULL) {
        LOG(ERROR) << "Fail to get file size: " << file_name;
        hdfsCloseFile(fs, file);
        hdfsDisconnect(fs);
        return NULL;
      }
      HDFSRandomAccessFile *hdfs_random_access = new HDFSRandomAccessFile;
      hdfs_random_access->file_name_ = file_name;
      hdfs_random_access->fs_ = fs;
      hdfs_random_access->file_ = file;
      hdfs_random_access->size_ = file_info->mSize;
      hdfsFreeFileInfo(file_info, 1);
      return hdfs_random_access;
    }


    virtual Status Read(uint64_t offset, size_t n, Slice* result,
                        char* scratch) const {
      Status s;
      if (offset > size_) {
        LOG(ERROR) << "Offset greater than file size " << size_ << " file name " << file_name_ << " for reading";
        return Status::IOError("Offset greater than file size.");
      }
      if (hdfsSeek(fs_, file_, offset)) {
        LOG(ERROR) << "Fail to seek offset " << offset << " file " << file_name_ << " for reading";
        return Status::IOError("seek error.");
      }

      size_t total = 0;
      while (total < n) {
        tSize  r = hdfsRead(fs_, file_, scratch + total, n - total);
        if (r < 0) {
          break;
        }
        total += r;
      }
      *result = Slice(scratch, total);
      LOG_IF(ERROR, total != n) << "required n " << n << " vs total " << total;
      return s;
    };
  };
}; // namespace leveldb

SSTable *SSTable::OpenFromHDFSFile(const std::string &file) {
  leveldb::HDFSRandomAccessFile *random_access_file = leveldb::HDFSRandomAccessFile::Open(file);
  if (random_access_file == NULL) {
    LOG(ERROR) << "SSTable::Open " << file << " NewRandomAccessFile fail ";
    return NULL;
  }
  leveldb::Table *table = NULL;
  leveldb::Options options;
  leveldb::Status status = leveldb::Table::Open(options, random_access_file,
      random_access_file->size(), &table);
  if (!status.ok()) {
    LOG(ERROR) << "Table::Open fail, status: " << status.ToString();
    return NULL;
  }
  SSTable *sstable(new SSTable);
  sstable->table_.reset(table);
  sstable->random_access_file_.reset(random_access_file);
  sstable->filename_ = file;
  return sstable;
}

SSTable *SSTable::OpenFromFile(const std::string &filename) {
  if (filename.size() >= 7u && filename.substr(0, 7) == "hdfs://") {
    return SSTable::OpenFromHDFSFile(filename);
  }
  return SSTable::Open(filename);
}
