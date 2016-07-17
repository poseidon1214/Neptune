#!/bin/sh

acl_bin=./zk_acl_admin_tool
node_bin=./zk_node_admin_tool
work_credential="zkwork:WorkHardPlayHarder"
work_digest="zkwork:+RIe5Gu+VpblWG/zCItFlEIHPOA="

if [ $# -ne 2 ]; then
    echo "Usage:"
    echo "    $0 nodename zkadmin:passwd"
    exit 1
fi

parent=`dirname $1`
$acl_bin --cmd=add --name=$parent --id=$work_digest --acl_version=-1 --create --read --admin=false --credential=$2
if [ $? -ne 0 ]; then
    echo "Run \"$acl_bin --cmd=add --name=$parent --id=$work_digest --acl_version=-1 --create --read --admin=false --credential=$2\" Failed"
    exit 1
fi

notexist="Node "$1" not exists"
$node_bin --name=$1 --credential=$work_credential 2>&1 | grep "$notexist" > /dev/null
if [ $? -ne 0 ]; then
    echo "Run: \"$node_bin --name=$1 --credential=$work_credential\" Failed"
    exit 1
fi

parent=`dirname $1`

$node_bin --cmd=create --name=$1 --credential=$work_credential &&\
$acl_bin --cmd=remove --name=$1 --id=$work_digest  --acl_version=-1 --credential=$work_credential &&\
$acl_bin --cmd=remove --name=$parent --id=$work_digest --acl_version=-1 --credential=$2
