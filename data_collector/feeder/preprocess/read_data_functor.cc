// Copyright (c) 2015, Tencent Inc.
// Author: cernwang <cernwang@tencent.com>

#include "data_collector/feeder/preprocess/read_data_functor.h"

#include <time.h>
#include "common/reader/reader.h"
#include "data_collector/feeder/common/rules_common.h"



namespace gdt {
namespace wavelet {

bool ReadDataFunctor::Init() {
  for (auto website_config : config_->website_config()) {
    source_to_index_[website_config.source_id()] = website_config.product_reader_config_index();
  }
  return true;
}

FunctorResult ReadDataFunctor::DoWork(DataMessage* data_message) {
  std::for_each(data_message->feeder_files.begin(),
                data_message->feeder_files.end(),
  	            std::bind(&ReadDataFunctor::GetProductFromFeedFile,
                          this,
                          std::placeholders::_1,
                          &(data_message->products)));
  std::for_each(data_message->products.begin(),
                data_message->products.end(),
                [&](Product& product) {
                  product.set_id(rules::ConvertToProductId(product.outer_id(), 
                                                           product.source_id()));
                });                
  return kSuccess;
}

bool ReadDataFunctor::GetProductFromFeedFile(
    const FeederFile& feeder_file,
    std::vector<Product>* whole_products) {
  std::vector<Product> products;
  PASS_OR_RETURN(feeder_file.downloaded());
  auto iter = source_to_index_.find(feeder_file.source_id());
  PASS_OR_RETURN(iter != source_to_index_.end());
  PASS_OR_RETURN(iter->second < config_->product_reader_config_size());
  auto product_reader_config = config_->product_reader_config(iter->second);
  if (!Reader::ReadFromFile(feeder_file.filename(),
                            product_reader_config.parse_config(),
                            &products)) {
    remove(feeder_file.filename().c_str());
    return false;
  }
  std::for_each(products.begin(),
                products.end(),
                [&](Product& product) {
                  product.set_source_id(feeder_file.source_id());
                });
  std::copy(products.begin(), products.end(), std::back_inserter(*whole_products));
  return true;
}
  
}  // namespace wavelet
}  // namespace gdt
