#!/bin/sh

if [ $# -lt 1 ]; then
    echo "Usage:"
    echo "  sh $0 user:passwd"
    exit 0
fi

get_install_path()
{
    filename="${BASH_SOURCE-$0}"
    filedir=`dirname ${filename}`
    echo `cd $filedir/..; pwd`
}

install_path=${install_path:-`get_install_path`}

source ${install_path}/bin/zk_env.sh

ZOOBIN=$zk_path/bin
ZOOBINDIR="$(cd "${ZOOBIN}"; pwd)"
source $zk_path/bin/zkEnv.sh

$JAVA_HOME/bin/java -cp $CLASSPATH org.apache.zookeeper.server.auth.DigestAuthenticationProvider $1
