#ifndef APP_QZAP_COMMON_SSTABLE_SSTABLE_H_
#define APP_QZAP_COMMON_SSTABLE_SSTABLE_H_
#include <stdint.h>
#include <string>
#include "app/qzap/common/base/scoped_ptr.h"
#include "thirdparty/leveldb/options.h"
namespace leveldb {
class Table;
class TableBuilder;
class RandomAccessFile;
class WritableFile;
class Iterator;
class Env;
}

// About SSTable:
//   http://www.quora.com/What-is-an-SSTable-in-Googles-internal-infrastructure.
class SSTable {
  public:
    class Iterator {
     public:
      ~Iterator();
      // An iterator is either positioned at a key/value pair, or
      // not valid.  This method returns true iff the iterator is valid.
      bool Valid() const;

      // Position at the first key in the source.  The iterator is Valid()
      // after this call iff the source is not empty.
      void SeekToFirst();

      // Position at the last key in the source.  The iterator is
      // Valid() after this call iff the source is not empty.
      void SeekToLast();

      // Position at the first key in the source that at or past target
      // The iterator is Valid() after this call iff the source contains
      // an entry that comes at or past target.
      void Seek(const std::string & target);

      // Moves to the next entry in the source.  After this call, Valid() is
      // true iff the iterator was not positioned at the last entry in the source.
      // REQUIRES: Valid()
      void Next();

      // Moves to the previous entry in the source.  After this call, Valid() is
      // true iff the iterator was not positioned at the first entry in source.
      // REQUIRES: Valid()
      void Prev();

      // Return the key for the current entry.  The underlying storage for
      // the returned slice is valid only until the next modification of
      // the iterator.
      // REQUIRES: Valid()
      std::string key() const;

      // Return the value for the current entry.  The underlying storage for
      // the returned slice is valid only until the next modification of
      // the iterator.
      // REQUIRES: !AtEnd() && !AtStart()
      std::string value() const;

      // status in string format.
      std::string status_string() const;
     private:
      Iterator();
      scoped_ptr<leveldb::Iterator> iterator_;
      friend class SSTable;
    };

    ~SSTable();
    static SSTable *Open(const std::string &file);
    static SSTable *OpenFromString(const std::string &input);
    static SSTable *OpenFromArray(const char *input, size_t len);
    static SSTable *OpenFromHDFSFile(const std::string &file);
    static SSTable* OpenFromFile(const std::string &file);
    Iterator *NewIterator() const;
  private:
    SSTable();
    scoped_ptr<leveldb::Table> table_;
    scoped_ptr<leveldb::RandomAccessFile> random_access_file_;
    std::string filename_;
};

class SSTableBuilder {
  public:
    explicit SSTableBuilder(const std::string &file);
    SSTableBuilder(const std::string &file, const leveldb::Options &options);
    ~SSTableBuilder();
    bool Add(const std::string &key, const std::string &value);
    bool Build();
    uint64_t NumEntries() const;
    uint64_t FileSize() const;
  private:
    bool Init(const leveldb::Options &options);
    std::string file_;
    scoped_ptr<leveldb::TableBuilder> table_builder_;
    scoped_ptr<leveldb::WritableFile> writable_file_;
    leveldb::Env *env_;
    leveldb::Options options_;
};
#endif  // APP_QZAP_COMMON_SSTABLE_SSTABLE_H_
