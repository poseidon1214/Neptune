#!/bin/sh

#call zkServer.sh

get_install_path()
{
    filename="${BASH_SOURCE-$0}"
    filedir=`dirname ${filename}`
    echo `cd $filedir/..; pwd`
}

install_path=${install_path:-`get_install_path`}

source ${install_path}/bin/zk_env.sh

${zk_path}/bin/zkServer.sh $1 ${install_path}/conf/zoo.cfg
