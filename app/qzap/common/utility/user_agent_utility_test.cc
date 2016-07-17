// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
#include <vector>
#include "app/qzap/common/utility/user_agent.pb.h"
#include "app/qzap/common/utility/user_agent_utility.h"
#include "app/qzap/proto/common/qzap_common.pb.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

namespace QZAP {
class UserAgentTest : public testing::Test {
};

TEST_F(UserAgentTest, TestUnknown) {
  UserAgent useragent;
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_UNKNOWN);
  ASSERT_EQ(useragent.screen_depth(), USERAGENT_SCREENDEPTH_UNKNOWN);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_UNKNOWN);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_UNKNOWN);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_UNKNOWN);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_UNKNOWN);
  ASSERT_EQ(useragent.connection_type(), CONNECTIONTYPE_UNKNOWN);
}

TEST_F(UserAgentTest, TestBrowser) {
  UserAgent useragent;
  ParseUserAgentString("travler", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_UNKNOWN);

  ParseUserAgentString("qqBrowser,tencenttravler", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_QQBROWSER);
  ParseUserAgentString("qqbrowser", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_QQBROWSER);
  ParseUserAgentString("tencenttraveler", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_TENCENTTRAVELER);
  ParseUserAgentString("360se", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_360SE);
  ParseUserAgentString("se 2.x metasr 1.0", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_SOUGOU);

  ParseUserAgentString("myie", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_MYIE);
  ParseUserAgentString("maxthon", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_MAXTHON);
  ParseUserAgentString("netscape", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_NETSCAPE);
  ParseUserAgentString("netcaptor", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_NETCAPTOR);
  ParseUserAgentString("theworld", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_THEWORLD);
  ParseUserAgentString("chrome", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_CHROME);
  ParseUserAgentString("firefox", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_FIREFOX);
  ParseUserAgentString("safari", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_SAFARI);
  ParseUserAgentString("opera", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_OPERA);
  ParseUserAgentString("gecko", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_GECKO);
  ParseUserAgentString("galeon", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_GALEON);
  ParseUserAgentString("nutscrape", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_NUTSCRAPE);
  ParseUserAgentString("greenbrowser", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_GREENBROWSER);
  ParseUserAgentString("msie 8", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_MSIE8);
  ParseUserAgentString("msie 7", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_MSIE7);
  ParseUserAgentString("msie 6", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_MSIE6);
  ParseUserAgentString("msie 5", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_MSIE5);
  ParseUserAgentString("msie 9", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_MSIE9);
}

TEST_F(UserAgentTest, TestScreenDepth) {
  UserAgent useragent;
  ParseUserAgentString("64-bit", &useragent);
  ASSERT_EQ(useragent.screen_depth(), USERAGENT_SCREENDEPTH_BIT_64);
  ParseUserAgentString("32-bit", &useragent);
  ASSERT_EQ(useragent.screen_depth(), USERAGENT_SCREENDEPTH_BIT_32);
  ParseUserAgentString("24-bit", &useragent);
  ASSERT_EQ(useragent.screen_depth(), USERAGENT_SCREENDEPTH_BIT_24);
  ParseUserAgentString("16-bit", &useragent);
  ASSERT_EQ(useragent.screen_depth(), USERAGENT_SCREENDEPTH_BIT_16);
  ParseUserAgentString("8-bit", &useragent);
  ASSERT_EQ(useragent.screen_depth(), USERAGENT_SCREENDEPTH_BIT_8);
  ParseUserAgentString("4-bit", &useragent);
  ASSERT_EQ(useragent.screen_depth(), USERAGENT_SCREENDEPTH_BIT_4);
  ParseUserAgentString("1-bit", &useragent);
  ASSERT_EQ(useragent.screen_depth(), USERAGENT_SCREENDEPTH_BIT_1);
}

TEST_F(UserAgentTest, TestPlatformType) {
  UserAgent useragent;
  ParseUserAgentString("win32", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_WIN32);
  ParseUserAgentString("win64", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_WIN64);
  ParseUserAgentString("windows", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_WINDOWS);
  ParseUserAgentString("x11", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_X11);
  ParseUserAgentString("ipad", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_IPAD);
  ParseUserAgentString("iphone", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_IPHONE);
  ParseUserAgentString("ipod", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_IPOD);
  ParseUserAgentString("macintel", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_MACINTEL);
  ParseUserAgentString("mac", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_MAC);
  ParseUserAgentString("nokia", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_SYMBIAN);
  ParseUserAgentString("s40", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_SYMBIAN);
  ParseUserAgentString("s60", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_SYMBIAN);
  ParseUserAgentString("series", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_SYMBIAN);
  ParseUserAgentString("symbian", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_SYMBIAN);
  ParseUserAgentString("blackberry", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_BLACKBERRY);
  ParseUserAgentString("freebsd", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_FREEBSD);
  ParseUserAgentString("pike", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_PIKE);
  ParseUserAgentString("android", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_ANDROID);
  ParseUserAgentString("wince", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_WINCE);
  ParseUserAgentString("mocor", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_MOCOR);
  ParseUserAgentString("sunos", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_SUNOS);
  ParseUserAgentString("ios", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_IOS);
  ParseUserAgentString("linux", &useragent);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_LINUX);
}

TEST_F(UserAgentTest, TestMobileOS) {
  UserAgent useragent;
  ParseUserAgentString("ios", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_IOS);
  ParseUserAgentString("ipad", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_IOS);
  ParseUserAgentString("iphone", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_IOS);
  ParseUserAgentString("ipod", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_IOS);
  ParseUserAgentString("mac", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_IOS);
  ParseUserAgentString("adr", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_ANDROID);
  ParseUserAgentString("android", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_ANDROID);
  ParseUserAgentString("nokia", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_SYMBIAN);
  ParseUserAgentString("s40", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_SYMBIAN);
  ParseUserAgentString("s60", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_SYMBIAN);
  ParseUserAgentString("series", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_SYMBIAN);
  ParseUserAgentString("symbian", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_SYMBIAN);
  ParseUserAgentString("blackberry", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_BLACKBERRY);
  ParseUserAgentString("wince", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_WINCE);
  ParseUserAgentString("windows ce", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_WINCE);
  ParseUserAgentString("windows mobile", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_WINCE);
  ParseUserAgentString("windows phone", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_WINCE);
  ParseUserAgentString("linux", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_ANDROID);
  ParseUserAgentString("x11", &useragent);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_ANDROID);
}

TEST_F(UserAgentTest, TestMobileType) {
  UserAgent useragent;
  ParseUserAgentString("nokia", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_NOKIA);
  ParseUserAgentString("s40", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_NOKIA);
  ParseUserAgentString("s60", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_NOKIA);
  ParseUserAgentString("series", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_NOKIA);
  ParseUserAgentString("symbian", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_NOKIA);
  ParseUserAgentString("desire", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_HTC);
  ParseUserAgentString("htc", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_HTC);
  ParseUserAgentString("ios", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_APPLE);
  ParseUserAgentString("ipad", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_APPLE);
  ParseUserAgentString("iphone", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_APPLE);
  ParseUserAgentString("ipod", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_APPLE);
  ParseUserAgentString("mac", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_APPLE);
  ParseUserAgentString("blackberry", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_BLACKBERRY);
  ParseUserAgentString("gt-", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_SAMSUNG);
  ParseUserAgentString("nexus", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_SAMSUNG);
  ParseUserAgentString("samsung", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_SAMSUNG);
  ParseUserAgentString("sgh", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_SAMSUNG);
  ParseUserAgentString("zte", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_ZTE);
  ParseUserAgentString("c8", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_HUAWEI);
  ParseUserAgentString("huawei", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_HUAWEI);
  ParseUserAgentString("hw", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_HUAWEI);
  ParseUserAgentString("u8", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_HUAWEI);
  ParseUserAgentString("sonyericsson", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_SONYERICSSON);
  ParseUserAgentString("3gw100", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_LENOVO);
  ParseUserAgentString("lenovo", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_LENOVO);
  ParseUserAgentString("oppo", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_OPPO);
  ParseUserAgentString("mb", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_MOTO);
  ParseUserAgentString("me", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_MOTO);
  ParseUserAgentString("milestone", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_MOTO);
  ParseUserAgentString("mot-", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_MOTO);
  ParseUserAgentString("xt", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_MOTO);
  ParseUserAgentString("mi-one", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_MIUI);
  ParseUserAgentString("bbk", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_BBK);
  ParseUserAgentString("vivo", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_BBK);
  ParseUserAgentString("m9", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_MEIZU);
  ParseUserAgentString("meizu", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_MEIZU);
  ParseUserAgentString("iuc", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_SHARP);
  ParseUserAgentString("nec", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_NEC);
  ParseUserAgentString("lg-", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_LG);
  ParseUserAgentString("hs", &useragent);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_HISENSE);
}

TEST_F(UserAgentTest, TestMobileBrowser) {
  UserAgent useragent;
  ParseUserAgentString("htc mqqbrowser", &useragent);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_QQBROWSER);
  ParseUserAgentString("htc chrome", &useragent);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_CHROME);
  ParseUserAgentString("htc uc browser", &useragent);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_UCWEB);
  ParseUserAgentString("htc ucweb", &useragent);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_UCWEB);
  ParseUserAgentString("htc gobrowser", &useragent);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_GOBROWSER);
  ParseUserAgentString("htc iemobile", &useragent);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_IEMOBILE);
  ParseUserAgentString("htc maui", &useragent);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_MAUIWAPBROWSER);
  ParseUserAgentString("htc opera", &useragent);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_OPERA);
  ParseUserAgentString("htc 360browser", &useragent);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_360BROWSER);
  ParseUserAgentString("htc mobile safari", &useragent);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_ANDROID);
  ParseUserAgentString("htc safari", &useragent);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_SAFARI);
  ParseUserAgentString("htc gecko", &useragent);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_GECKO);
  ParseUserAgentString("htc mozilla", &useragent);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_MOZILLA);
}

TEST_F(UserAgentTest, TestRealCase) {
  UserAgent useragent;
  ParseUserAgentString(
    "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Trident/4.0; "
    ".NET CLR 2.0.50727)",
    &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_MSIE7);
  ASSERT_EQ(useragent.screen_depth(), USERAGENT_SCREENDEPTH_UNKNOWN);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_WINDOWS);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_UNKNOWN);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_UNKNOWN);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_MOZILLA);
  ParseUserAgentString(
      "Nokia5800w/52.0.007 (SymbianOS/9.4; U; Series60/5.0; Mozilla/5.0; "
      "Profile/MIDP-2.1 Configuration/CLDC-1.1) "
      "AppleWebKit/413 (KHTML,like Gecko) Safari/413 3gpp-gba", &useragent);
  ASSERT_EQ(useragent.browser(), USERAGENT_BROWSER_SAFARI);
  ASSERT_EQ(useragent.screen_depth(), USERAGENT_SCREENDEPTH_UNKNOWN);
  ASSERT_EQ(useragent.platform_type(), USERAGENT_PLATFORMTYPE_SYMBIAN);
  ASSERT_EQ(useragent.mobile_os(), USERAGENT_MOBILEOS_SYMBIAN);
  ASSERT_EQ(useragent.mobile_type(), USERAGENT_MOBILETYPE_NOKIA);
  ASSERT_EQ(useragent.mobile_browser(), USERAGENT_MOBILEBROWSER_SAFARI);
}

TEST_F(UserAgentTest, TestNetworkType) {
  UserAgent useragent;
  ParseUserAgentString("netType/WIFI WebP", &useragent);
  ASSERT_EQ(useragent.connection_type(), CONNECTIONTYPE_WIFI);
  ParseUserAgentString("NetType/2G WebP", &useragent);
  ASSERT_EQ(useragent.connection_type(), CONNECTIONTYPE_2G);
  ParseUserAgentString("NetType/3G WebP", &useragent);
  ASSERT_EQ(useragent.connection_type(), CONNECTIONTYPE_3G);
  ParseUserAgentString("NetType/4g WebP", &useragent);
  ASSERT_EQ(useragent.connection_type(), CONNECTIONTYPE_4G);
}
}  // namespace QZAP
