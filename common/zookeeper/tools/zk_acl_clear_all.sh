#!/bin/sh

acl_bin=./zk_acl_admin_tool
digest_line="Auth info"

if [ $# -ne 2 ]; then
    echo "Usage:"
    echo "    $0 nodename username:passwd"
    exit 1
fi

for line in `$acl_bin --name=$1 2>&1 | grep ^I | grep "$digest_line" | awk -F "$digest_line" '{print $2}' | awk '{print $2","$3}'  | sort | uniq`;
do
    if [ "$line" == "world,anyone" ]; then
        continue
    fi
    echo "===to delete: "$line
    scheme=`echo $line | awk -F "," '{print $1}'`
    id=`echo $line | awk -F "," '{print $2}'`
    $acl_bin --cmd=remove --name=$1 --credential=$2 --acl_version=-1 --scheme=$scheme --id=$id
    if [ $? -ne 0 ]; then
        echo "===failed"
        exit 1
    fi
    echo "===succ"
done

$acl_bin --name=$1
