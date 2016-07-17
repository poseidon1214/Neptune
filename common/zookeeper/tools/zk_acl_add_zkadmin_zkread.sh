#!/bin/sh

acl_bin=./zk_acl_admin_tool
zkadmin="zkadmin:TncLYUw1TXJ8gggTgS0nagVumY8="
zkread="zkread:GcFRWbb6OPSLn2eATJPgIvJuf1c="

if [ $# -ne 1 ]; then
    echo "Usage:"
    echo "    $0 nodename"
    exit 1
fi

$acl_bin --cmd=add --name=$1 --id=$zkadmin --acl_version=-1 &&\
$acl_bin --cmd=add --name=$1 --id=$zkread --read --admin=false --acl_version=-1 &&\
$acl_bin --cmd=remove --name=$1 --scheme=world --acl_version=-1 &&\
$acl_bin --name=$1
