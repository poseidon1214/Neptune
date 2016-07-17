// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-4-17
#ifndef APP_QZAP_COMMON_UTILITY_LOG_TNM_UTILITY_H_
#define APP_QZAP_COMMON_UTILITY_LOG_TNM_UTILITY_H_

#include <map>
#include <string>

namespace google {
  class LogSink;
};

void InitTNMLogSink(int attr_id);

// read log_tnm_pool_tester.cc for more detail
void InitTNMLogSinkPool(const std::map<std::string, int>& key_to_attr_id);
::google::LogSink* TNMSink(const std::string& key);
#endif  // APP_QZAP_COMMON_UTILITY_LOG_TNM_UTILITY_H_
