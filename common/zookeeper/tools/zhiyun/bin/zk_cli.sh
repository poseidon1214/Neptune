#!/bin/sh

#call zkCli.sh

get_install_path()
{
    filename="${BASH_SOURCE-$0}"
    filedir=`dirname ${filename}`
    echo `cd $filedir/..; pwd`
}

install_path=${install_path:-`get_install_path`}

source ${install_path}/bin/zk_env.sh

if [ $# -gt 0 ]; then
    server=$1
else
    port=`cat ${install_path}/conf/zoo.cfg | grep ^clientPort | awk -F "=" '{print $2}'`
    server=${myip}:${port}
fi

${zk_path}/bin/zkCli.sh -server ${server}
