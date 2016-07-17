#!/bin/sh

#cd $PATH_QZONE_PRJ/app/qzap/service/adsyncnew/
#make

_path=$PATH_QZONE_PRJ/app/qzap/service/adsyncnew/qzap_svr_adsync

cd $_path/bin
./stop.sh all

cd $_path
make clean;make

cd $_path/bin
ulimit -c unlimited
./start.sh all
