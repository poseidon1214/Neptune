// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
#ifndef APP_QZAP_COMMON_UTILITY_USER_AGENT_UTILITY_H_
#define APP_QZAP_COMMON_UTILITY_USER_AGENT_UTILITY_H_
namespace QZAP {
class UserAgent;
void ParseUserAgentString(
  const std::string &user_agent_string,
  UserAgent *user_agent);
}
#endif // APP_QZAP_COMMON_UTILITY_USER_AGENT_UTILITY_H_

