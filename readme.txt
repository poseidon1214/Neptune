工程简列如下：不清楚时请及时与justinyang沟通，谢谢

└─trunk
    ├─app				应用层业务代码集合
    │  ├─broadcast			业务(空间广播站)代码
    │  │  ├─cgi				web cgi(空间广播站)代码
    │  │  │  ├─bin
    │  │  │  ├─cgi-bin
    │  │  │  ├─htdocs			网页及js等相关（一般我们采有AJAX放在CDN上）
    │  │  │  └─tools
    │  │  ├─common			业务级共用库 纵向功能api
    │  │  │  ├─env			本业务对于的配置文件目录(与环境相关)
    │  │  │  ├─include
    │  │  │  ├─lib
    │  │  │  ├─obj
    │  │  │  └─src			当功能多时，这里按功能来分目录组织源文件(必要时加上命名空间，命名空间取名为业务名，首写字母为大写如Broadcast)
 
    │  │  └─servers			对外服务相关代码（可按子系统再细分子目录，子系统目录下再放相关的模块代码，模块代码的结构详见https://tc-svn/isd/isd_qzoneact_rep/adsdoc_proj/trunk/小组规范/后台进程相关约定.txt）
    │  │      ├─qbs_svr_cpx
    │  │      │  ├─bin
    │  │      │  ├─data
    │  │      │  ├─etc
    │  │      │  ├─invoice
    │  │      │  ├─log
    │  │      │  ├─src
    │  │      │  │  ├─lib
    │  │      │  │  ├─spp_incl
    │  │      │  │  └─tools
    │  │      │  └─tools
    │  │      ├─qbs_svr_ctrl
    │  │      │  ├─bin
    │  │      │  ├─conf
    │  │      │  ├─data
    │  │      │  ├─invoice
    │  │      │  ├─log
    │  │      │  ├─src
    │  │      │  │  ├─include
    │  │      │  │  ├─lib
    │  │      │  │  ├─obj
    │  │      │  │  ├─server
    │  │      │  │  │  └─spp_incl
    │  │      │  │  └─tools
    │  │      │  └─tools
    │  │      └─qbs_svr_slave
    │  │          ├─bin
    │  │          ├─conf
    │  │          ├─data
    │  │          ├─invoice
    │  │          ├─log
    │  │          ├─src
    │  │          │  ├─include
    │  │          │  ├─lib
    │  │          │  ├─obj
    │  │          │  ├─server
    │  │          │  │  └─spp_incl
    │  │          │  └─tools
    │  │          └─tools
    │ 
    ├─base_class			工程基础库
    │  ├─extern_libs			引入第三方库编译后的目标目录：按功能划分子目录（与源代码目录名相同），每个目录下有include:头文件 lib:库文件。
    │  │  ├─agent
    │  │  │  ├─include
    │  │  │  └─lib
    │  │  ├─c4a
    │  │  │  ├─bin
    │  │  │  │  ├─conf
    │  │  │  │  └─log
    │  │  │  ├─include
    │  │  │  ├─lib
    │  │  │  └─src
    │  ├─extern_src			引入的第三方库源代码目录：按功能划分子目录
    │  │  ├─cookie
    │  │  └─tdev
    │  │      └─comm
    │  │          ├─cgihtml
    │  │          ├─include
    │  │          ├─lib
    │  │          └─tlib
    │  ├─include			我方库编译后的头文件目录
    │  ├─lib				我方库编译后的库文件目录
    │  ├─obj
    │  └─src				我方库的源代码目录：按功能划分目录
    │      ├─bitmap_client
    │      ├─cgiex
    │      ├─cma_client
    │      ├─cmss
    ├─data
    ├─dirty_check
    ├─etc				运营时相关配置文件目录：按业务划分目录，详见备注说明
    ├─log				运营时相关日志文件目录
    ├─makeinclude			工程编译总makefile
    ├─release				web服务相关运营目录:ARS发布软链的父目录，业务代码编译后执行文件到相关的目录，外网实际运营时release改为app
    │  ├─broadcast.qq.com		业务相关的域名，外网实际运营时目录名为业务名
    │  │  ├─bin
    │  │  ├─cgi-bin
    │  │  ├─fcg-bin
    │  │  ├─htdocs
    │  │  └─tools
    │  │
    │  └─ep.qq.com
    │      ├─bin
    │      ├─cgi-bin
    │      ├─fcg-bin
    │      ├─htdocs
    │      └─tools
    
 

备注：
一.
	1.一个业务下多域名如何处理？

		用makefile来解决，make all是编译所有执行文件，采用make install分发到相关的release域名下

	2.一域名在多业务下如何处理?
		
		应当尽量避免，出现时用makefile来解决，make all是编译所有执行文件，采用make install分发到相关的release域名下
	
	3.后台程序各开发阶段如何处理？
		
		开发阶段，在代码目录进行调试。
		测试及运营目录，按照BU的要求进行，详见https://tc-svn/isd/isd_qzoneact_rep/adsdoc_proj/trunk/小组规范/后台进程相关约定.txt

	4.总makefile的功能？
		
		对工程进行总的编译
		将相关的执行文件发布到release相关目录
		将工程目录下运营相关目录如etc,data在release相关目录下建立软软接

二.
	1.一级目录etc下按业务放目录存放各自应用的配置，目录名为业务名，即与app下目录名一致
	2.每个业务目录下的配置有TTC DB及其它等 命名为{业务名}_ttc/db_
	3.业务代码common/env存放相关的配置文件头文件

三。
	1.文件名命名以业务名(业务的简称)开头
	2.命名空间为业务全称，首字母大写
