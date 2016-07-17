// Copyright (c) 2011, Tencent Inc.  All rights reserved.
// Author: Wenting Liu <wentingliu@tencent.com>
// Created: 11/11/15
// Description: common rsync class

#ifndef COMMON_NET_RSYNC_RSYNC_H_
#define COMMON_NET_RSYNC_RSYNC_H_
#pragma once

#include <string>
#include <vector>

namespace gdt {

class Rsync {
 public:
  class Options {
   public:
    Options()
      : timeout_s(30), archive_mode(true), compress(true), append(false) {
    }

    Options& SetTimeout(int timeout_s) {
      this->timeout_s = timeout_s;
      return *this;
    }

    Options& SetArchiveMode(bool archive_mode) {
      this->archive_mode = archive_mode;
      return *this;
    }

    Options& SetCompress(bool compress) {
      this->compress = compress;
      return *this;
    }

    Options& SetAppend(bool append) {
      this->append = append;
      return *this;
    }

   public:
    int timeout_s;
    bool archive_mode;
    bool compress;
    bool append;
  };

 public:
  Rsync();
  explicit Rsync(const Options& options);
  // pull single file or directory.
  std::string Pull(
      // remote_address should be in format ip:port, pass in "" for local test
      const std::string& remote_address,
      // remote rsync daemon source path, starts with modname
      const std::string& source,
      const std::string& destination);
  // pull multiple files to local folder
  std::string Pull(
      const std::string& remote_address,
      const std::vector<std::string>& sources,
      const std::string& destination);

 private:
  void Initialize(const Options& options);
  void ParseOptions();
  std::string AggregateSources(const std::vector<std::string>& sources);
  std::string BuildCommand(
      const std::string& remote_address,
      const std::string& source,
      const std::string& destination);
  std::string GetErrorMessage(FILE* popen_error);

 private:
  Options options_;
  std::string parsed_options_;
};

}  // namespace gdt
#endif  // COMMON_NET_RSYNC_RSYNC_H_
