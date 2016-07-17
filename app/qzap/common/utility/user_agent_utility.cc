// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22
/*
	查找的关键字	匹配到的类型
--------------------------------------
浏览器:
	qqbrowser	QQBrowser
	tencenttraveler	TencentTraveler
	360se	360SE
	se 2.x metasr 1.0	SouGou
	myie	MyIE
	maxthon	Maxthon
	netscape	Netscape
	netcaptor	NetCaptor
	theworld	TheWorld
	chrome	Chrome
	firefox	Firefox
	safari	Safari
	opera	Opera
	gecko	Gecko
	galeon	Galeon
	nutscrape	Nutscrape
	greenbrowser	GreenBrowser
	msie 8	MSIE8
	msie 7	MSIE7
	msie 6	MSIE6
	msie 5	MSIE5
	msie 9	MSIE9
---------------------------------------
屏幕颜色:
        32-bit	32-bit
	64-bit	64-bit
	24-bit	24-bit
	16-bit	16-bit
	8-bit	8-bit
	4-bit	4-bit
	1-bit	1-bit
---------------------------------------
平台类型:
        win32	win32
	win64	win64
	windows	windows
	x11	x11
	ipad	ipad
	iphone	iphone
	ipod	ipod
	macintel	macintel
	mac	mac
	nokia	symbian
	s40	symbian
	s60	symbian
	series	symbian
	symbian	symbian
	blackberry	blackberry
	freebsd	freebsd
	pike	pike
	android	Android
	wince	WinCE
	mocor	Mocor
	sunos	SunOS
	ios	ios
	linux	Linux
---------------------------------------
移动操作系统:
	ios	iOS
	ipad	iOS
	iphone	iOS
	ipod	iOS
	mac	iOS
	adr	Android
	android	Android
	nokia	Symbian
	s40	Symbian
	s60	Symbian
	series	Symbian
	symbian	Symbian
	blackberry	BlackBerry OS
	wince	WinCE
	windows ce	WinCE
	windows mobile	WinCE
	windows phone	WinCE
	linux	Linux
	x11	Linux
---------------------------------------
手机型号:
	nokia	nokia
	s40	nokia
	s60	nokia
	series	nokia
	symbian	nokia
	desire	htc
	htc	htc
	ios	apple
	ipad	apple
	iphone	apple
	ipod	apple
	mac	apple
	blackberry	blackberry
	gt-	samsung
	nexus	samsung
	samsung	samsung
	sgh	samsung
	zte	zte
	c8	huawei
	huawei	huawei
	hw	huawei
	u8	huawei
	sonyericsson	sonyericsson
	3gw100	lenovo
	lenovo	lenovo
	oppo	oppo
	mb	moto
	me	moto
	milestone	moto
	mot-	moto
	xt	moto
	mi-one	miui
	bbk	bbk
	vivo	bbk
	m9	meizu
	meizu	meizu
	iuc	sharp
	nec	nec
	lg-	lg
	hs	hisense
---------------------------------------
手机浏览器:
	mqqbrowser	QQBrowser
	chrome	Chrome
	uc browser	UCWEB
	ucweb	UCWEB
	gobrowser	GoBrowser
	iemobile	IEMobile
	maui	MAUI WAP Browser
	opera	Opera
	360browser	360browser
	mobile safari	Android
	safari	Safari
	gecko	Gecko
	mozilla	Mozilla
---------------------------------------
*/
#include <string>

#include "app/qzap/common/base/string_utility.h"
#include "app/qzap/common/utility/user_agent.pb.h"
#include "app/qzap/common/utility/user_agent_utility.h"
#include "app/qzap/proto/common/qzap_common.pb.h"

namespace QZAP {
static const struct UserAgent_Browser_Match {
  const char *src;
  UserAgent_Browser dest;
} kUserAgentBrowserMatchTable[] = {
  {"qqbrowser", USERAGENT_BROWSER_QQBROWSER},
  {"tencenttraveler", USERAGENT_BROWSER_TENCENTTRAVELER},
  {"360se", USERAGENT_BROWSER_360SE},
  {"se 2.x metasr 1.0", USERAGENT_BROWSER_SOUGOU},
  {"myie", USERAGENT_BROWSER_MYIE},
  {"maxthon", USERAGENT_BROWSER_MAXTHON},
  {"netscape", USERAGENT_BROWSER_NETSCAPE},
  {"netcaptor", USERAGENT_BROWSER_NETCAPTOR},
  {"theworld", USERAGENT_BROWSER_THEWORLD},
  {"chrome", USERAGENT_BROWSER_CHROME},
  {"firefox", USERAGENT_BROWSER_FIREFOX},
  {"safari", USERAGENT_BROWSER_SAFARI},
  {"opera", USERAGENT_BROWSER_OPERA},
  {"gecko", USERAGENT_BROWSER_GECKO},
  {"galeon", USERAGENT_BROWSER_GALEON},
  {"nutscrape", USERAGENT_BROWSER_NUTSCRAPE},
  {"greenbrowser", USERAGENT_BROWSER_GREENBROWSER},
  {"msie 8", USERAGENT_BROWSER_MSIE8},
  {"msie 7", USERAGENT_BROWSER_MSIE7},
  {"msie 6", USERAGENT_BROWSER_MSIE6},
  {"msie 5", USERAGENT_BROWSER_MSIE5},
  {"msie 9", USERAGENT_BROWSER_MSIE9},
};


static const struct UserAgent_ScreenDepth_Match {
  const char *src;
  UserAgent_ScreenDepth dest;
} kUserAgentScreenDepthMatchTable[] = {
  {"64-bit", USERAGENT_SCREENDEPTH_BIT_64},
  {"32-bit", USERAGENT_SCREENDEPTH_BIT_32},
  {"24-bit", USERAGENT_SCREENDEPTH_BIT_24},
  {"16-bit", USERAGENT_SCREENDEPTH_BIT_16},
  {"8-bit", USERAGENT_SCREENDEPTH_BIT_8},
  {"4-bit", USERAGENT_SCREENDEPTH_BIT_4},
  {"1-bit", USERAGENT_SCREENDEPTH_BIT_1}
};

static const struct UserAgent_PlatformType_Match {
  const char *src;
  UserAgent_PlatformType dest;
} kUserAgentPlatformTypeMatchTable[] = {
  {"win32", USERAGENT_PLATFORMTYPE_WIN32},
  {"win64", USERAGENT_PLATFORMTYPE_WIN64},
  {"windows", USERAGENT_PLATFORMTYPE_WINDOWS},
  {"x11", USERAGENT_PLATFORMTYPE_X11},
  {"ipad", USERAGENT_PLATFORMTYPE_IPAD},
  {"iphone", USERAGENT_PLATFORMTYPE_IPHONE},
  {"ipod", USERAGENT_PLATFORMTYPE_IPOD},
  {"macintel", USERAGENT_PLATFORMTYPE_MACINTEL},
  {"mac", USERAGENT_PLATFORMTYPE_MAC},
  {"nokia", USERAGENT_PLATFORMTYPE_SYMBIAN},
  {"s40", USERAGENT_PLATFORMTYPE_SYMBIAN},
  {"s60", USERAGENT_PLATFORMTYPE_SYMBIAN},
  {"series", USERAGENT_PLATFORMTYPE_SYMBIAN},
  {"symbian", USERAGENT_PLATFORMTYPE_SYMBIAN},
  {"blackberry", USERAGENT_PLATFORMTYPE_BLACKBERRY},
  {"freebsd", USERAGENT_PLATFORMTYPE_FREEBSD},
  {"pike", USERAGENT_PLATFORMTYPE_PIKE},
  {"android", USERAGENT_PLATFORMTYPE_ANDROID},
  {"wince", USERAGENT_PLATFORMTYPE_WINCE},
  {"mocor", USERAGENT_PLATFORMTYPE_MOCOR},
  {"sunos", USERAGENT_PLATFORMTYPE_SUNOS},
  {"ios", USERAGENT_PLATFORMTYPE_IOS},
  {"linux", USERAGENT_PLATFORMTYPE_LINUX},
};

static const struct UserAgent_MobileOS_Match {
  const char *src;
  UserAgent_MobileOS dest;
} kUserAgentMobileOSMatchTable[] = {
  {"ios", USERAGENT_MOBILEOS_IOS},
  {"ipad", USERAGENT_MOBILEOS_IOS},
  {"iphone", USERAGENT_MOBILEOS_IOS},
  {"ipod", USERAGENT_MOBILEOS_IOS},
  {"mac", USERAGENT_MOBILEOS_IOS},
  {"adr", USERAGENT_MOBILEOS_ANDROID},
  {"android", USERAGENT_MOBILEOS_ANDROID},
  {"nokia", USERAGENT_MOBILEOS_SYMBIAN},
  {"s40", USERAGENT_MOBILEOS_SYMBIAN},
  {"s60", USERAGENT_MOBILEOS_SYMBIAN},
  {"series", USERAGENT_MOBILEOS_SYMBIAN},
  {"symbian", USERAGENT_MOBILEOS_SYMBIAN},
  {"blackberry", USERAGENT_MOBILEOS_BLACKBERRY},
  {"wince", USERAGENT_MOBILEOS_WINCE},
  {"windows ce", USERAGENT_MOBILEOS_WINCE},
  {"windows mobile", USERAGENT_MOBILEOS_WINCE},
  {"windows phone", USERAGENT_MOBILEOS_WINCE},
  {"linux", USERAGENT_MOBILEOS_ANDROID},
  {"maui", USERAGENT_MOBILEOS_JAVA},
  {"midp", USERAGENT_MOBILEOS_JAVA},
};

static const struct UserAgent_MobileType_Match {
  const char *src;
  UserAgent_MobileType dest;
} kUserAgentMobileTypeMatchTable[] = {
  {"nokia", USERAGENT_MOBILETYPE_NOKIA},
  {"s40", USERAGENT_MOBILETYPE_NOKIA},
  {"s60", USERAGENT_MOBILETYPE_NOKIA},
  {"series", USERAGENT_MOBILETYPE_NOKIA},
  {"symbian", USERAGENT_MOBILETYPE_NOKIA},
  {"desire", USERAGENT_MOBILETYPE_HTC},
  {"htc", USERAGENT_MOBILETYPE_HTC},
  {"ios", USERAGENT_MOBILETYPE_APPLE},
  {"ipad", USERAGENT_MOBILETYPE_APPLE},
  {"iphone", USERAGENT_MOBILETYPE_APPLE},
  {"ipod", USERAGENT_MOBILETYPE_APPLE},
  {"mac", USERAGENT_MOBILETYPE_APPLE},
  {"blackberry", USERAGENT_MOBILETYPE_BLACKBERRY},
  {"gt-", USERAGENT_MOBILETYPE_SAMSUNG},
  {"nexus", USERAGENT_MOBILETYPE_SAMSUNG},
  {"samsung", USERAGENT_MOBILETYPE_SAMSUNG},
  {"sgh", USERAGENT_MOBILETYPE_SAMSUNG},
  {"zte", USERAGENT_MOBILETYPE_ZTE},
  {"c8", USERAGENT_MOBILETYPE_HUAWEI},
  {"huawei", USERAGENT_MOBILETYPE_HUAWEI},
  {"hw", USERAGENT_MOBILETYPE_HUAWEI},
  {"u8", USERAGENT_MOBILETYPE_HUAWEI},
  {"sonyericsson", USERAGENT_MOBILETYPE_SONYERICSSON},
  {"3gw100", USERAGENT_MOBILETYPE_LENOVO},
  {"lenovo", USERAGENT_MOBILETYPE_LENOVO},
  {"oppo", USERAGENT_MOBILETYPE_OPPO},
  {"mb", USERAGENT_MOBILETYPE_MOTO},
  // meizu shoud be in the front of me otherwise meizu will hit me.
  {"meizu", USERAGENT_MOBILETYPE_MEIZU},
  {"me", USERAGENT_MOBILETYPE_MOTO},
  {"milestone", USERAGENT_MOBILETYPE_MOTO},
  {"mot-", USERAGENT_MOBILETYPE_MOTO},
  {"xt", USERAGENT_MOBILETYPE_MOTO},
  {"mi-one", USERAGENT_MOBILETYPE_MIUI},
  {"bbk", USERAGENT_MOBILETYPE_BBK},
  {"vivo", USERAGENT_MOBILETYPE_BBK},
  {"m9", USERAGENT_MOBILETYPE_MEIZU},
  {"iuc", USERAGENT_MOBILETYPE_SHARP},
  {"nec", USERAGENT_MOBILETYPE_NEC},
  {"lg-", USERAGENT_MOBILETYPE_LG},
  {"hs", USERAGENT_MOBILETYPE_HISENSE},
};

static const struct UserAgent_MobileBrowser_Match {
  const char *src;
  UserAgent_MobileBrowser dest;
} kUserAgentMobileBrowserMatchTable[] = {
  {"mqqbrowser", USERAGENT_MOBILEBROWSER_QQBROWSER},
  {"chrome", USERAGENT_MOBILEBROWSER_CHROME},
  {"uc browser", USERAGENT_MOBILEBROWSER_UCWEB},
  {"ucweb", USERAGENT_MOBILEBROWSER_UCWEB},
  {"gobrowser", USERAGENT_MOBILEBROWSER_GOBROWSER},
  {"iemobile", USERAGENT_MOBILEBROWSER_IEMOBILE},
  {"maui", USERAGENT_MOBILEBROWSER_MAUIWAPBROWSER},
  {"opera", USERAGENT_MOBILEBROWSER_OPERA},
  {"360browser", USERAGENT_MOBILEBROWSER_360BROWSER},
  {"mobile safari", USERAGENT_MOBILEBROWSER_ANDROID},
  {"safari", USERAGENT_MOBILEBROWSER_SAFARI},
  {"gecko", USERAGENT_MOBILEBROWSER_GECKO},
  {"mozilla", USERAGENT_MOBILEBROWSER_MOZILLA},
};

static const struct UserAgent_ConnectionType_Match {
  const char *src;
  ConnectionType dest;
} kUserAgentConnectionTypeMatchTable[] = {
  {"nettype/wifi", CONNECTIONTYPE_WIFI},
  {"nettype/2g", CONNECTIONTYPE_2G},
  {"nettype/3g", CONNECTIONTYPE_3G},
  {"nettype/4g", CONNECTIONTYPE_4G},
};

void ParseUserAgentString(const std::string &user_agent_string,
                          UserAgent *user_agent) {
  std::string lower_user_agent(user_agent_string);
  StringToLower(&lower_user_agent);
  for (int i = 0; i < arraysize(kUserAgentBrowserMatchTable); ++i) {
    if (lower_user_agent.find(kUserAgentBrowserMatchTable[i].src)
        != std::string::npos) {
      user_agent->set_browser(kUserAgentBrowserMatchTable[i].dest);
      break;
    }
  }

  for (int i = 0; i < arraysize(kUserAgentScreenDepthMatchTable); ++i) {
    if (lower_user_agent.find(kUserAgentScreenDepthMatchTable[i].src)
        != std::string::npos) {
      user_agent->set_screen_depth(kUserAgentScreenDepthMatchTable[i].dest);
      break;
    }
  }

  for (int i = 0; i < arraysize(kUserAgentPlatformTypeMatchTable); ++i) {
    if (lower_user_agent.find(kUserAgentPlatformTypeMatchTable[i].src)
        != std::string::npos) {
      user_agent->set_platform_type(kUserAgentPlatformTypeMatchTable[i].dest);
      break;
    }
  }

  for (int i = 0; i < arraysize(kUserAgentMobileTypeMatchTable); ++i) {
    if (lower_user_agent.find(kUserAgentMobileTypeMatchTable[i].src)
        != std::string::npos) {
      user_agent->set_mobile_type(kUserAgentMobileTypeMatchTable[i].dest);
      break;
    }
  }

  // 目前移动定向主要关注操作系统，尚不关注手机品牌，这里的逻辑会使操作系统
  // 未知数大大增加， 因此先注释掉  neoli 201304017
/*
  if (user_agent->mobile_type() == USERAGENT_MOBILETYPE_UNKNOWN) {
    // do not match other mobile information if can't find the mobile type.
    // otherwise there will be some false positive match, e.g. 'linux' will
    // match the mobile os to be linux.
    return;
  }
*/ 
  for (int i = 0; i < arraysize(kUserAgentMobileOSMatchTable); ++i) {
    if (lower_user_agent.find(kUserAgentMobileOSMatchTable[i].src)
        != std::string::npos) {
      user_agent->set_mobile_os(kUserAgentMobileOSMatchTable[i].dest);
      break;
    }
  }

  for (int i = 0; i < arraysize(kUserAgentMobileBrowserMatchTable); ++i) {
    if (lower_user_agent.find(kUserAgentMobileBrowserMatchTable[i].src)
        != std::string::npos) {
      user_agent->set_mobile_browser(kUserAgentMobileBrowserMatchTable[i].dest);
      break;
    }
  }

  for (int i = 0; i < arraysize(kUserAgentConnectionTypeMatchTable); ++i) {
    if (lower_user_agent.find(kUserAgentConnectionTypeMatchTable[i].src)
        != std::string::npos) {
      user_agent->set_connection_type(
          kUserAgentConnectionTypeMatchTable[i].dest);
      break;
    }
  }
}
}  // namespace QZAP
