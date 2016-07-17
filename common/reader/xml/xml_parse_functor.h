// Copyright (c) 2014 Tencent Inc.
// Author: cernwang (cernwang@tencent.com)

#ifndef COMMON_READER_XML_XML_READER_H_
#define COMMON_READER_XML_XML_READER_H_

#include <string>

namespace gdt {

class XmlParseFunctor : public DataParseFunctor {
 public:
  XmlParseFunctor() : DataParseFunctor() {
  }

  virtual ~XmlParseFunctor() {}

  virtual const std::string Name() {
    return "XmlParseFunctor";
  }

 private:
  // 处理网站级别的数据
  bool ParseCommoditiesFromDocument(
      const WebsiteConfig& website, std::vector<Commodity>* commodities);

  // 处理文件级别的数据
  bool ParseCommoditiesFromSingleDocument(
      const WebsiteConfig& website,
      const std::string& filename,
      std::vector<Commodity>* commodities,
      uint32_t* stat_str_to_num_error);
};

}  // namespace gdt

#endif  // COMMON_READER_XML_XML_READER_H_

