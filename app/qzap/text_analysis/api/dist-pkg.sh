#!/bin/bash

# Copyright (c) 2015 Tencent Inc.
# Author: Ping Wang (samulwang@tencent.com)

function prepare_inc {
    PROJ_HOME='../../../..'
    API_INC_DIR='./include'
    
    cp ../text_miner_resource.h ../text_miner.h .

    mkdir -p ./lib
    cp $PROJ_HOME/build64_release/app/qzap/text_analysis/libspa_text_miner.so ./lib/
    
    mkdir -p $API_INC_DIR/app/qzap/text_analysis
    cp $PROJ_HOME/build64_release/app/qzap/text_analysis/text_miner.pb.h $API_INC_DIR/app/qzap/text_analysis
    
    mkdir -p $API_INC_DIR/app/qzap/common/base
    cp $PROJ_HOME/app/qzap/common/base/base.h          $API_INC_DIR/app/qzap/common/base/
    cp $PROJ_HOME/app/qzap/common/base/scoped_ptr.h    $API_INC_DIR/app/qzap/common/base/
    cp $PROJ_HOME/app/qzap/common/base/static_assert.h $API_INC_DIR/app/qzap/common/base/
    
    mkdir -p $API_INC_DIR/app/qzap/common/thread
    cp $PROJ_HOME/app/qzap/common/thread/mutex.h $API_INC_DIR/app/qzap/common/thread/
    
    mkdir -p $API_INC_DIR/common/base/preprocess
    cp $PROJ_HOME/common/base/platform_features.h $API_INC_DIR/common/base/
    cp $PROJ_HOME/common/base/preprocess.h        $API_INC_DIR/common/base/
    cp $PROJ_HOME/common/base/scoped_ptr.h        $API_INC_DIR/common/base/
    cp $PROJ_HOME/common/base/singleton.h         $API_INC_DIR/common/base/
    cp $PROJ_HOME/common/base/uncopyable.h        $API_INC_DIR/common/base/
    cp $PROJ_HOME/common/base/platform_features.h $API_INC_DIR/common/base/
    cp $PROJ_HOME/common/base/static_assert.h     $API_INC_DIR/common/base/
    cp $PROJ_HOME/common/base/preprocess/disallow_in_header_file.h $API_INC_DIR/common/base/preprocess
    cp $PROJ_HOME/common/base/preprocess/join.h                    $API_INC_DIR/common/base/preprocess
    cp $PROJ_HOME/common/base/preprocess/stringize.h               $API_INC_DIR/common/base/preprocess
    cp $PROJ_HOME/common/base/preprocess/varargs.h                 $API_INC_DIR/common/base/preprocess
    
    mkdir -p $API_INC_DIR/common/system/concurrency
    cp $PROJ_HOME/common/system/check_error.h                    $API_INC_DIR/common/system/
    cp $PROJ_HOME/common/system/concurrency/condition_variable.h $API_INC_DIR/common/system/concurrency/
    cp $PROJ_HOME/common/system/concurrency/mutex.h              $API_INC_DIR/common/system/concurrency/
    cp $PROJ_HOME/common/system/concurrency/scoped_locker.h      $API_INC_DIR/common/system/concurrency/
    
    mkdir -p $API_INC_DIR/thirdparty/google/protobuf/io
    mkdir -p $API_INC_DIR/thirdparty/google/protobuf/stubs
    cp $PROJ_HOME/thirdparty/google/protobuf/*.h       $API_INC_DIR/thirdparty/google/protobuf/
    cp $PROJ_HOME/thirdparty/google/protobuf/stubs/*.h $API_INC_DIR/thirdparty/google/protobuf/stubs/
    cp $PROJ_HOME/thirdparty/google/protobuf/io/*.h    $API_INC_DIR/thirdparty/google/protobuf/io/
    mkdir -p $API_INC_DIR/thirdparty/protobuf-2.6.1/src/google/protobuf/io
    mkdir -p $API_INC_DIR/thirdparty/protobuf-2.6.1/src/google/protobuf/stubs
    cp $PROJ_HOME/thirdparty/protobuf-2.6.1/src/google/protobuf/*.h       $API_INC_DIR/thirdparty/protobuf-2.6.1/src/google/protobuf/
    cp $PROJ_HOME/thirdparty/protobuf-2.6.1/src/google/protobuf/stubs/*.h $API_INC_DIR/thirdparty/protobuf-2.6.1/src/google/protobuf/stubs/
    cp $PROJ_HOME/thirdparty/protobuf-2.6.1/src/google/protobuf/io/*.h    $API_INC_DIR/thirdparty/protobuf-2.6.1/src/google/protobuf/io/
    
    mkdir -p $API_INC_DIR/thirdparty/gflags
    cp $PROJ_HOME/thirdparty/gflags/*.h $API_INC_DIR/thirdparty/gflags/
    mkdir -p $API_INC_DIR/thirdparty/gflags-2.0/src/gflags
    cp $PROJ_HOME/thirdparty/gflags-2.0/src/gflags/*.h $API_INC_DIR/thirdparty/gflags-2.0/src/gflags/
    
    mkdir -p $API_INC_DIR/thirdparty/glog
    cp $PROJ_HOME/thirdparty/glog/*.h $API_INC_DIR/thirdparty/glog/
    mkdir -p $API_INC_DIR/thirdparty/glog-0.3.2/src/glog
    cp $PROJ_HOME/thirdparty/glog-0.3.2/src/glog/*.h $API_INC_DIR/thirdparty/glog-0.3.2/src/glog/
}

function make_tar {
    src='text_miner.h text_miner_resource.h text_miner_demo_main.cc text_miner.proto'
    tar zcvf text_miner.tgz $src data lib include Makefile readme.txt run_text_miner_demo_main.sh text_miner_resource.config
}

if [ $# -ne 1 ]; then echo "./Usage: $0 [inc|tar]"; exit -1; fi

if [ "$1" = "inc" ]; then prepare_inc;
elif [ "$1" = "tar" ]; then make_tar;
else echo "./Usage: $0 [inc|tar]"; exit -1
fi
