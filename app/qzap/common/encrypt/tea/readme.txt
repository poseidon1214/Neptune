tea 对称加密算法用于解密QQ客户端广告请求里的敏感信息

QQ客户端提供了原始加密解密的代码， 房租code_from_qq_client
代码提供者：joshualeung ramonji

为了能在后台服务器上使用， 封装成tea.h 和 tea.cc
使用时代码里include了 #include "app/qzap/common/encrypt/tea/tea.h"
链接库libtea.a, 目前只提供32bit版本 debug和release版本， 分别放在
app/qzap/common/encrypt/tea/lib32_release/libtea.a
app/qzap/common/encrypt/tea/lib32_debug//libtea.a



