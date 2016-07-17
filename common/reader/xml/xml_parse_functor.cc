// Copyright (c) 2015, Tencent Inc.
// Author: cernwang <cernwang@tencent.com>

#include "creative/dynamic_creative/preprocess/xml_parse_functor.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/libxml/HTMLparser.h"

DECLARE_string(data_dir);

namespace gdt {
namespace dynamic_creative {

// 从Xml中获取商品的信息
bool XmlParseFunctor::ParseCommoditiesFromDocument(
    const WebsiteConfig& website, std::vector<Commodity>* commodities) {

  // 获取网站简称
  const std::string& website_short_name = website.website_short_name();
  // 获取文件类型
  WebsiteConfig_DocumentType document_type = website.document_type();

  if (document_type != WebsiteConfig_DocumentType_XML &&
      document_type != WebsiteConfig_DocumentType_XML_SET) {
    return true;
  }

  LOG(INFO) << "document_type:" << document_type;
  // 获取文件路径
  const std::string& filename =
      OfflineCommon::GenerateDataFileName(website_short_name, FLAGS_data_dir,
                                          document_type);
  uint32_t stat_str_to_num_error = 0;
  if (document_type == WebsiteConfig_DocumentType_XML) {
    return ParseCommoditiesFromSingleDocument(website, filename,
           commodities, &stat_str_to_num_error);
  } else if (document_type == WebsiteConfig_DocumentType_XML_SET) {
    uint32_t origin_size = commodities->size();
    std::vector<std::string> files;
    GlobInDirs(filename + "/*", 0, &files);
    if (files.size() == 0) {
      LOG(ERROR) << "File dir Empty:" << filename;
      return false;
    }
    uint32_t parse_failed = 0;
    for (uint32_t i = 0; i < files.size(); i++) {
      if (!ParseCommoditiesFromSingleDocument(website, files[i],
          commodities, &stat_str_to_num_error)) {
        parse_failed++;
      }
    }
    LOG(INFO) << website_short_name << " commodity size:" << (commodities->size() - origin_size);
    LOG(INFO) << "Total commodity size:" << commodities->size();
    if (static_cast<double>(parse_failed) / files.size() > 0.5) {
      return false;
    } else {
      return true;
    }
  }
  return true;
}

bool XmlParseFunctor::ParseCommoditiesFromSingleDocument(
    const WebsiteConfig& website,
    const std::string& filename,
    std::vector<Commodity>* commodities,
    uint32_t* stat_str_to_num_error) {
  uint32_t origin_size = commodities->size();
  // XML是否存在
  if (!FileExisting(filename.c_str())) {
    LOG(ERROR) << " Xml file not exist :" << filename;
    return false;
  }

  LOG(INFO) << "Parse Commodites From " << filename << "...";

  xmlDocPtr doc;
  xmlNodePtr current_node;
  doc = xmlReadFile(filename.c_str(), "UTF8", XML_PARSE_RECOVER);
  if (doc == NULL) {
    LOG(ERROR) << "Xml not parsed successfully. " << filename;
    return false;
  }
  current_node = xmlDocGetRootElement(doc);
  if (current_node == NULL) {
    LOG(ERROR) << "Empty root element. :" << filename;
    xmlFreeDoc(doc);
    return false;
  }
  if (xmlStrcasecmp(current_node->name, BAD_CAST "urlset")) {
    xmlFreeDoc(doc);
    return false;
  }

  current_node = current_node -> xmlChildrenNode;
  while (current_node != NULL) {
    if (current_node->type == XML_ELEMENT_NODE) {
      if ((!xmlStrcasecmp(current_node->name, (const xmlChar *)"url"))) {
        xmlNodePtr url_current_node = current_node->xmlChildrenNode;
        Commodity commodity;
        std::string style = "", materialquality = "";
        MaterailExt& extend_info = (*commodity.mutable_extend_info());
        bool has_str_to_num_error = false;
        // 商品页面url
        while (url_current_node != NULL) {
          if ((!xmlStrcasecmp(url_current_node->name, (const xmlChar *)"loc"))) {
            commodity.set_url(
              reinterpret_cast<char*>(
                xmlNodeGetContent(url_current_node)));
          } else if ((!xmlStrcasecmp(url_current_node->name,
                                 (const xmlChar *)"data"))) {
            xmlNodePtr data_current_node = url_current_node->xmlChildrenNode;
            while (data_current_node != NULL) {
              if ((!xmlStrcasecmp(data_current_node->name,
                              (const xmlChar *)"name"))) {
                // 商品名称
                commodity.set_commodity_name(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"accountID"))) {
                // 广告主ID
                uint64_t advertiser_id;
                if (StringToNumeric(reinterpret_cast<char*>(
                                    xmlNodeGetContent(data_current_node)), &advertiser_id)) {
                  commodity.set_advertiser_id(advertiser_id);
                }
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"title"))) {
                // 商品页面url
                commodity.set_commodity_description(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"outerID"))) {
                std::string outer_id_str(reinterpret_cast<char*>(xmlNodeGetContent(data_current_node)));
                commodity.set_outer_id_str(outer_id_str);
                // 商品外部ID
                uint64_t outer_id = OfflineCommon::ConvertToOuterID(outer_id_str);
                commodity.set_outer_id(outer_id);
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"sellerName"))) {
                // 商品站点名称
                extend_info.set_website_name(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                     (const xmlChar *)"sellerSiteUrl")) ||
                         (!xmlStrcasecmp(data_current_node->name,
                                     (const xmlChar *)"sellerSiteurl"))) {
                // 商品站点url
                extend_info.set_website_url(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"sellerSiteLogoUrl"))) {
                // 商品站点url
                extend_info.set_website_logo_url(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"image"))) {
                // 商品站点url
                extend_info.set_image_url(
                  OfflineCommon::NormalizeUrl(reinterpret_cast<char*>(
                                              xmlNodeGetContent(data_current_node))));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"price"))) {
                // 商品现价
                double price;
                if (!StringToNumeric(reinterpret_cast<char*>(
                                     xmlNodeGetContent(data_current_node)), &price)) {
                  data_current_node = data_current_node->next;
                  has_str_to_num_error = true;
                  break;
                }
                commodity.set_price(price);
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"value"))) {
                // 商品原价
                double origin_price;
                if (!StringToNumeric(reinterpret_cast<char*>(
                                     xmlNodeGetContent(data_current_node)), &origin_price)) {
                  data_current_node = data_current_node->next;
                  has_str_to_num_error = true;
                  break;
                }
                extend_info.set_origin_price(origin_price);
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"brand"))) {
                // 商品品牌
                commodity.set_commodity_brand(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"brandUrl")) ||
                         (!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"brandurl"))) {
                // 品牌页url
                commodity.set_commodity_brand_target_url(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"targetUrl")) ||
                         (!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"targeturl"))) {
                // 创意跳转的目标网址
                commodity.set_target_url(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"category"))) {
                // 一级分类
                commodity.set_first_category(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"subCategory"))) {
                // 二级分类
                commodity.set_second_category(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"thirdCategory"))) {
                // 三级分类
                commodity.set_third_category(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"fourthCategory"))) {
                // 三级分类
                commodity.set_fourth_category(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"categoryUrl"))) {
                // 一级分类url
                commodity.set_first_category_url(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"subCategoryUrl"))) {
                // 二级分类url
                commodity.set_second_category_url(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"thirdCategoryUrl"))) {
                // 三级分类url
                commodity.set_third_category_url(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"fourthCategoryUrl"))) {
                // 三级分类url
                commodity.set_fourth_category_url(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"model"))) {
                // 型号
                extend_info.set_product_model(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"stock"))) {
                // 库存量
                int32_t stock_volume;
                if (!StringToNumeric(reinterpret_cast<char*>(
                                     xmlNodeGetContent(data_current_node)), &stock_volume)) {
                  data_current_node = data_current_node->next;
                  has_str_to_num_error = true;
                  break;
                }
                commodity.set_stock_volume(abs(stock_volume));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"attribute"))) {
                // 属性
                commodity.set_attribute(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"bought"))) {
                // 销量
                int32_t sales_volume;
                if (!StringToNumeric(reinterpret_cast<char*>(
                                     xmlNodeGetContent(data_current_node)), &sales_volume)) {
                  data_current_node = data_current_node->next;
                  has_str_to_num_error = true;
                  break;
                }
                commodity.set_sales_volume(abs(sales_volume));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"categoryId"))) {
                // 类目ID
                int32_t outer_category_id;
                if (!StringToNumeric(reinterpret_cast<char*>(
                                     xmlNodeGetContent(data_current_node)), &outer_category_id)) {
                  data_current_node = data_current_node->next;
                  has_str_to_num_error = true;
                  break;
                }
                commodity.set_outer_category_id(outer_category_id);
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"label"))) {
                // 商品标签
                std::string labels_str(reinterpret_cast<char*>(
                                           xmlNodeGetContent(data_current_node)));
                std::vector<std::string> labels;
                SplitString(labels_str, ",", &labels);
                for (int i = 0; i < labels.size(); i++) {
                  commodity.add_advertiser_labels(labels[i]);
                }
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"style"))) {
                style = std::string(reinterpret_cast<char*>(xmlNodeGetContent(
                                                            data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"materialquality"))) {
                materialquality =
                  std::string(reinterpret_cast<char*>(xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"targetUrlMobile"))) {
                commodity.set_mobile_target_url(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"targetUrlApp"))) {
                commodity.set_app_target_url(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"shopID"))) {
                // 店铺ID
                uint64_t shop_id;
                if (StringToNumeric(reinterpret_cast<char*>(
                                    xmlNodeGetContent(data_current_node)), &shop_id)) {
                  commodity.mutable_shop_data()->set_shop_id(shop_id);
                }
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"shopName"))) {
                commodity.mutable_shop_data()->set_shop_name(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"shopUrl"))) {
                commodity.mutable_shop_data()->set_shop_url(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"shopAttribute"))) {
                commodity.mutable_shop_data()->set_shop_attribute(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"city"))) {
                commodity.set_city(
                    reinterpret_cast<char*>(
                        xmlNodeGetContent(data_current_node)));
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"priceApp"))) {
                double price;
                if (!StringToNumeric(reinterpret_cast<char*>(
                                     xmlNodeGetContent(data_current_node)), &price)) {
                  data_current_node = data_current_node->next;
                  has_str_to_num_error = true;
                  break;
                }
                commodity.set_price_app(price);
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"pricePC"))) {
                double price;
                if (!StringToNumeric(reinterpret_cast<char*>(
                                     xmlNodeGetContent(data_current_node)), &price)) {
                  data_current_node = data_current_node->next;
                  has_str_to_num_error = true;
                  break;
                }
                commodity.set_price_pc(price);
              } else if ((!xmlStrcasecmp(data_current_node->name,
                                        (const xmlChar *)"priceH5"))) {
                double price;
                if (!StringToNumeric(reinterpret_cast<char*>(
                                     xmlNodeGetContent(data_current_node)), &price)) {
                  data_current_node = data_current_node->next;
                  has_str_to_num_error = true;
                  break;
                }
                commodity.set_price_h5(price);
              }
              data_current_node = data_current_node->next;
            }
          }
          url_current_node = url_current_node->next;
        }
        if (has_str_to_num_error) {
          current_node = current_node->next;
          (*stat_str_to_num_error)++;
          continue;
        }
        // 填充网站基本基本信息
        if (!FillWebsiteBaseInfo(website, &commodity)) {
          continue;
        }
        // 填充商品基本信息
        FillCommodityBaseInfo(&commodity);
        // 插入到商品数据中
        (*commodities).push_back(commodity);
      }
    }
    current_node = current_node->next;
  }
  xmlFreeDoc(doc);
  LOG(INFO) << filename << ":" << (commodities->size() - origin_size);

  return true;
}

}  // namespace dynamic_creative
}  // namespace gdt
