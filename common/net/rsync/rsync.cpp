// Copyright (c) 2011, Tencent Inc.  All rights reserved.
// Author: Wenting Liu <wentingliu@tencent.com>
// Created: 11/11/15
// Description: common rsync class

#include "common/net/rsync/rsync.h"

#include <stdio.h>
#include <iterator>
#include <string>
#include <vector>

#include "app/qzap/common/base/string_utility.h"
#include "common/base/string/concat.h"
#include "common/encoding/shell.h"

namespace gdt {

Rsync::Rsync() {
  Initialize(Options());
}

Rsync::Rsync(const Options& options) {
  Initialize(options);
}

void Rsync::Initialize(const Options& options) {
  this->options_ = options;
  ParseOptions();
}

void Rsync::ParseOptions() {
  parsed_options_ = parsed_options_ + "-";
  if (this->options_.archive_mode) {
    parsed_options_ = parsed_options_ + "a";
  }
  if (this->options_.compress) {
    parsed_options_ = parsed_options_ + "z";
  }
  if (parsed_options_ == "-") {
    parsed_options_ = "";
  }
  StringAppend(&parsed_options_, " --timeout=", options_.timeout_s);
  if (this->options_.append) {
    parsed_options_ = parsed_options_ + " --append";
  }
}

std::string Rsync::AggregateSources(
    const std::vector<std::string>& sources) {
  std::string joined_sources = JoinCommandLine(sources);
  return "'" + joined_sources + "'";
}

std::string Rsync::BuildCommand(
    const std::string& remote_address,
    const std::string& source,
    const std::string& destination) {
  std::string parsed_remote_address;
  if (remote_address == "") {
    parsed_remote_address = "";
  } else {
    parsed_remote_address = parsed_remote_address + "rsync://" + remote_address;
    char ending = remote_address.at(remote_address.length() - 1);
    if (ending != '/') {
      parsed_remote_address = parsed_remote_address + "/";
    }
  }
  std::string cmd;
  StringAppend(&cmd, "rsync", " ", parsed_options_, " ",
      parsed_remote_address, source, " ", destination, " 2>&1 >/dev/null");
  return cmd;
}

std::string Rsync::GetErrorMessage(FILE* popen_error) {
  std::string error_message;
  char line_buffer[4096];
  while (!feof(popen_error) && fgets(line_buffer,
      sizeof(line_buffer), popen_error) != NULL) {
    error_message += line_buffer;
  }
  return error_message;
}

std::string Rsync::Pull(
    const std::string& remote_address,
    const std::vector<std::string>& sources,
    const std::string& destination) {
  std::string source = AggregateSources(sources);
  return Pull(remote_address, source, destination);
}

std::string Rsync::Pull(
    const std::string& remote_address,
    const std::string& source,
    const std::string& destination) {
  std::string cmd = BuildCommand(remote_address, source, destination);
  FILE* popen_error = popen(cmd.c_str(), "r");
  std::string error_message = GetErrorMessage(popen_error);
  pclose(popen_error);
  return error_message;
}

}  // namespace gdt
