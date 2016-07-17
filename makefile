#注意顺序前三个extern_libs base init请不要更改

target=extern_libs base init compo tubi qzap express brand portal
target_clean=base_clean extern_libs_clean compo_clean qzap_clean express_clean tubi_clean brand_clean portal_clean

ifdef BIGQQ_FLAGS
	_base_path=base_class
else
	_base_path=base_class_old
endif

all:$(target)

init:
	cd shell;make init;make all;make install
	cd resource;make express
	cd resource;make brand

extern_libs:
	cd $(_base_path)/extern_src;make all;
#	cd $(_base_path)/extern_libs/qq_acct/;make all;
	
base:
	cd $(_base_path)/src;make all; 

compo:
	cd components; make all;

qzap:
	cd app/qzap/; make all;
	
express:
	cd app/express/;make all;

tubi:
	cd app/tubi/;make all;

brand:
	cd app/brand/;make all;

portal:
	cd app/portal/;make all;

install:
	echo "now checking env...."
	chmod -R 777 release/log/

extern_libs_clean:
	cd $(_base_path)/extern_src;make clean;

base_clean:
	cd $(_base_path)/src;make clean;

compo_clean:
	cd components; make clean;

qzap_clean:
	cd app/qzap/; make clean;
	
express_clean:
	cd app/express/;make clean;

tubi_clean:
	cd app/tubi/;make clean;

brand_clean:
	cd app/brand/;make clean;
portal_clean:
	cd app/portal/;make clean;
clean:$(target_clean)

blade_build:
	env LANG="zh_CN.UTF-8" LC_ALL="zh_CN.UTF-8" BLADE_AUTO_UPGRADE=no ./blade build ... -j 1
blade_clean:
	env LANG="zh_CN.UTF-8" LC_ALL="zh_CN.UTF-8" BLADE_AUTO_UPGRADE=no ./blade clean ...
blade_test:
	env LANG="zh_CN.UTF-8" LC_ALL="zh_CN.UTF-8" BLADE_AUTO_UPGRADE=no ./blade test ... -j 1

