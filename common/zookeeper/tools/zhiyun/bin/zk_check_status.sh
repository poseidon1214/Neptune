#!/bin/sh

#call zkServer.sh status 

# crontab env
export PATH=$PATH:/usr/local/bin:/usr/local/sbin:/usr/sbin:/sbin:$HOME/bin

get_install_path()
{
    filename="${BASH_SOURCE-$0}"
    filedir=`dirname ${filename}`
    echo `cd $filedir/..; pwd`
}
install_path=${install_path:-`get_install_path`}

opt_cmd_agent_str=/usr/local/agenttools/agent/agentRepStr
function xalarm()
{
    local attr=$1
    shift
    local msg="$*"
    if [ ${#msg} -gt 120 ]; then
        msg=${msg: -120:120}
    fi
    $opt_cmd_agent_str $attr "${msg}"
}

enable_jmx_bk=$enable_jmx
unset enable_jmx
source ${install_path}/bin/zk_env.sh

source ${install_path}/conf/alarmid
alarmid=${alarmid:-0}

${zk_path}/bin/zkServer.sh status ${install_path}/conf/zoo.cfg 1>/dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "[`date "+%Y-%m-%d %H:%M:%S"`] Fail to check status, alarm(${alarmid})"
    xalarm $alarmid "Fail to check zk status" 1>/dev/null
else
    echo "[`date "+%Y-%m-%d %H:%M:%S"`] Check zk status succ"

    # check warning log
    warn_log=${install_path}/log/zookeeper.warn.log
    last_modify=${install_path}/log/warnlog_lastmodify
    if [ ! -f $warn_log ]; then
        echo "[`date "+%Y-%m-%d %H:%M:%S"`] Fail to find '${warn_log}'"
        exit 0
    fi

    if [ -f $last_modify ]; then
        m0=`cat $last_modify`
        m1=`stat $warn_log | grep ^Modify`
        if [ "${m0}" == "${m1}" ]; then
            echo "[`date "+%Y-%m-%d %H:%M:%S"`] '${warn_log}' unchanged since '$m0'"
            exit 0
        fi
    fi

    stat $warn_log | grep ^Modify > $last_modify

    filter_file=${install_path}/bin/filter_warns
    if [ ! -f $filter_file ]; then
        echo "fjdsiofoafjDSAFDSF" > $filter_file
    fi
        
    last_line=`awk 'FNR==NR { map[$0]="" }
        FNR!=NR { if (substr($0, 0, 1)=="2") { for (l in map) if (match($0, l)){next} print}}' $filter_file $warn_log | tail --line 1`
    if [ -z "$last_line" ]; then
        echo "[`date "+%Y-%m-%d %H:%M:%S"`] No warnings is being found in '${warn_log}'"
        exit 0
    fi

    last_alarm=${install_path}/log/warnlog_lastalarm
    if [ -f $last_alarm ]; then
        a0=`cat $last_alarm`
        if [ "${a0}" == "${last_line}" ]; then
            echo "[`date "+%Y-%m-%d %H:%M:%S"`] The log has been alarmed, '$last_line'"
            exit 0
        fi
    fi
    echo "$last_line" > $last_alarm
    echo "[`date "+%Y-%m-%d %H:%M:%S"`] alarm($alarmid): $last_line"
    xalarm $alarmid "$last_line" 1>/dev/null

    exit 0
fi

if [ "x$1" = "xcheck_and_restart" ]; then
    echo "[`date "+%Y-%m-%d %H:%M:%S"`] Restart zk"
    # If you start with 'enable_jmx=true' and then stopped by accident, the restart would happen automatically, but 'enable_jmx' is not setted
    # I don't know how to pass this variable when crontab called which added by zhiyun's admin.
    # I don't think it is a important thing either.
    enable_jmx=$enable_jmx_bk
    source ${install_path}/bin/zk_env.sh
    ${zk_path}/bin/zkServer.sh restart ${install_path}/conf/zoo.cfg
fi
exit 1
