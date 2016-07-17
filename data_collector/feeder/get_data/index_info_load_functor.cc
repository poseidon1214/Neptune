// Copyright (c) 2015, Tencent Inc.
// Author: cernwang <cernwang@tencent.com>

#include "data_collector/feeder/get_data/index_info_load_functor.h"

#include <time.h>
#include "common/file/file_tools.h"
#include "common/reader/reader.h"
#include "data_collector/feeder/common/rules_common.h"

namespace gdt {

bool IndexInfoLoadFunctor::Init() {
  downloader_.Init();
  return true;
}

FunctorResult IndexInfoLoadFunctor::DoWork(DataMessage* data_message) {
  std::for_each(config_->website_config().begin(),
                config_->website_config().end(),
                std::bind(&IndexInfoLoadFunctor::FillFeederFile, this,
                          std::placeholders::_1, 
                          &(data_message->feeder_files)));
  std::for_each(data_message->feeder_files.begin(),
                data_message->feeder_files.end(),
                [&](FeederFile& feeder_file) {
                  feeder_file.set_filename(rules::GenerateFilename(feeder_file.url(), 
                                                                   config_->data_dir() + "/" + feeder_file.source()));
                });
  return kSuccess;
}

bool IndexInfoLoadFunctor::FillFeederFile(
    const WebsiteConfig& website_config,
    std::vector<FeederFile>* whole_feeder_files) {
  std::vector<FeederFile> feeder_files;
  REGISTER_FUNC(rules::GetUnixTime);
  std::string dir = config_->data_dir() + "/" + website_config.name();
  std::string url = website_config.get_data_config().url_pattern();
  FileTools::MakeDir(dir);
  if (website_config.get_data_config().is_index_file()) {
    PASS_OR_RETURN(website_config.get_data_config().index_reader_config_index() < 
                   config_->index_reader_config_size());
    auto index_reader_config = config_->index_reader_config(website_config.get_data_config().index_reader_config_index());
    std::string index_file = dir + "/index.xml";
    if (!downloader_.DoDownloadFile(url, index_file)) {
      remove(index_file.c_str());
      return false;
    }
    if (!Reader::ReadFromFile(index_file,
                              index_reader_config.parse_config(),
                              &feeder_files)) {
      remove(index_file.c_str());
      return false;
    }
  } else {
    FeederFile feeder_file;
    feeder_file.set_url(url);
    time_t now;
    time(&now);
    feeder_file.set_last_modify_time(now);
    feeder_files.push_back(feeder_file);
  }
  LOG(ERROR) << website_config.name();
  std::for_each(feeder_files.begin(),
                feeder_files.end(),
                [&](FeederFile& feeder_file) {
                  feeder_file.set_source(website_config.name());
                  feeder_file.set_source_id(website_config.source_id());
                });
  std::copy(feeder_files.begin(), feeder_files.end(), std::back_inserter(*whole_feeder_files));
  return true;
}

}  // namespace gdt
