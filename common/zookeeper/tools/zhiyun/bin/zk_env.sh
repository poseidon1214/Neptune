#!/bin/sh

export myip=`ifconfig | grep "inet addr" | awk '{split($2, a, ":"); if (a[1]="addr") print a[2]}' | grep -v "127.0.0.1" | head --line 1`

export JAVA_HOME=${install_path}/jdk1.8.0_65
export zk_path=${install_path}/zookeeper-3.4.6

export ZOO_LOG_DIR=${install_path}/log
export ZOO_LOG4J_PROP="INFO, INFOFILE, WARNFILE"
export ZOOCFGDIR=${install_path}/conf
export SERVER_JVMFLAGS="-Dzookeeper.DigestAuthenticationProvider.superDigest=super:oeagHZ6Mo3T86eW3SHIWcs5h6rg="

if [ "x$enable_jmx" != "x" ]; then
    jmx_port=${jmx_port:-"8080"}
    export SERVER_JVMFLAGS="${SERVER_JVMFLAGS} -Dcom.sun.management.jmxremote.port=${jmx_port} -Dcom.sun.management.jmxremote.rmi.port=${jmx_port} -Djava.rmi.server.hostname=${myip} -Dcom.sun.management.jmxremote.ssl=false"
    export JMXDISABLE=""
else
    export JMXDISABLE="true"
fi 

