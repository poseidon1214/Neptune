// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)

#ifndef RETRIEVAL_EXTRACTOR_EXTRACTOR_FUNCTOR_H_
#define RETRIEVAL_EXTRACTOR_EXTRACTOR_FUNCTOR_H_

#include <vector>
#include <string>

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/base/scoped_ptr.h"
#include "app/qzap/common/utility/file_utility.h"
#include "app/qzap/text_analysis/text_miner.h"
#include "app/qzap/text_analysis/text_miner_resource.h"
#include "app/qzap/text_analysis/segmenter/segmenter.h"
#include "common/reader/reader.h"
#include "common/encoding/pb_to_map.h"
#include "data_collector/feeder/common/rules_common.h"
#include "retrieval/extractor/data_message.h"

namespace gdt {

class ExtractFunctor: public ExtractorBaseFunctor {
 public:
  // 初始化
  bool Init() {
    if (config_->has_text_miner_config_file()) {
      text_miner_resource_.reset(new qzap::text_analysis::TextMinerResource());
      CHECK(!text_miner_resource_->InitFromConfigFile(config_->text_miner_config_file()));
      text_miner_.reset(new qzap::text_analysis::TextMiner(text_miner_resource_.get()));    
    }
    CHECK(segmenter.Init());
    return true;  
  }

  // 从文件里解析商品数据
  FunctorResult DoWork(ExtratorDataMessage* data_message) {
    std::for_each(data_message->products.begin(),
    	            data_message->products.end(),
    	            [=](const Product& product) {
    	          	  Document document;
    	          	  Convert(product, &document);
                    data_message->documents.push_back(document);
    	            });
    return kSuccess;
  }
 private:
  // 讲消息转换成文档
  bool Convert(const Product& product, Document* document) {
    std::map<std::string, std::string> parameters;
    ProtoMessageToMap(product, &parameters);
    for (auto field_config : config_->field_config()) {
      auto iter = parameters.find(field_config.field_name());
      CHECK_CONTINUE(iter != parameters.end());
      auto field = document->add_field();
      CHECK_CONTINUE(ExtractField(iter->second, field_config, field));
    }
    return true;
  }

  // 讲消息转换成文档
  bool ExtractField(const std::string& value,
                    const FieldExatractConfig field_config,
                    Field* field) {
    switch (field_config.extract_method()) {
      case SEGMENT_WORD:
        field->set_field_type(TYPE_ID);
        return ExtractSegmentWordField(value, field);
      case STRING_TO_NUMERIC:
        double num;
        field->set_field_type(TYPE_NUM);
        CHECK(StringToNumeric(value, &num));
        field->set_num(num);
      case STRING_TO_ID:
        uint64_t id;
        CHECK(StringToNumeric(value, &id));
        field->set_field_type(TYPE_ID);
        field->add_id(id);
        break;
      case MD5_STRING_TO_ID:
        field->set_field_type(TYPE_ID);
        field->add_id(gdt::rules::MD5(value));
        break;
    }
    return true;
  }
 
  // 抽取分词结果
  bool ExtractSegmentWordField(const std::string& value,
                               Field* field) {
    std::vector<std::string> words;
    CHECK(!segmenter.Segment(value, &words));
    for (auto word : words) {
      uint64_t id;
      field->add_id(gdt::rules::MD5(word));
    }
    return true; 
  }

 private:
  // 资源
  scoped_ptr<qzap::text_analysis::TextMinerResource> text_miner_resource_;
  // 文本分析器
  scoped_ptr<qzap::text_analysis::TextMiner> text_miner_;
  // 分词器
  qzap::text_analysis::Segmenter segmenter;
};

}  // namespace gdt

#endif  // RETRIEVAL_EXTRACTOR_EXTRACTOR_FUNCTOR_H_
