#!/bin/sh

#1.mkdir dataDir; conf dataDir
#2.mkdir dataLogDir; conf dataLogDir
#3.touch myid

get_install_path()
{
    filename="${BASH_SOURCE-$0}"
    filedir=`dirname ${filename}`
    echo `cd $filedir/..; pwd`
}
install_path=${install_path:-`get_install_path`}

cd ${install_path} && tar -zxf server-jre-8u65-linux-x64.gz
cd ${install_path} && tar -zxf zookeeper-3.4.6.tar.gz

get_name()
{
    tmp_name=`echo $install_path | awk -F "/" '{print $NF}'`
    echo $tmp_name | grep zookeeper > /dev/null
    ret=$?
    if [ $ret -ne 0 ]; then
        echo zookeeper_${tmp_name}
    else
        echo $tmp_name
    fi
}
name=${name:-`get_name`}

get_data_dir()
{
    disk_num=`df | grep data | wc -l`
    if [ $disk_num -gt 0 ]; then
        num=`df | grep data | awk -F "data" '{print $NF}' | sort -n | head --line 1`
        echo "/data$num/$name"
    else
        echo "$HOME/$name"
    fi
}

get_data_log_dir()
{
    disk_num=`df | grep data | wc -l`
    if [ $disk_num -gt 1 ]; then
        num=`df | grep data | awk -F "data" '{print $NF}' | sort -n | sed -n 2p`
        echo "/data$num/$name"
    elif [ $disk_num -gt 0 ]; then
        num=`df | grep data | awk -F "data" '{print $NF}' | sort -n | sed -n 1p`
        echo "/data$num/$name"
    else
        echo "$HOME/$name"
    fi
}

if [ -f $install_path/conf/zoo.cfg ]; then
    echo `date`
    data_dir=$(get_data_dir)
    data_log_dir=$(get_data_log_dir)

    #dataDir
    mkdir -p $data_dir
    ret=$?
    if [ $ret -eq 0 ]; then
        echo "mkdir dataDir: '${data_dir}' success"
    else
        echo "mkdir dataDir: '${data_dir}' fail"
    fi
    grep "dataDir=" $install_path/conf/zoo.cfg >/dev/null
    ret=$?
    if [ $ret -ne 0 ]; then
        echo "dataDir=${data_dir}" >> $install_path/conf/zoo.cfg
        ret=$?
    else
        sed -i "s%.*dataDir=.*%dataDir=${data_dir}%g" $install_path/conf/zoo.cfg
        ret=$?
    fi
    if [ $ret -eq 0 ]; then
        echo "config dataDir success"
    else
        echo "config dataDir fail"
    fi

    #dataLogDir
    mkdir -p $data_log_dir
    ret=$?
    if [ $ret -eq 0 ]; then
        echo "mkdir dataLogDir: '${data_log_dir}' success"
    else
        echo "mkdir dataLogDir: '${data_log_dir}' fail"
    fi
    grep "dataLogDir=" $install_path/conf/zoo.cfg >/dev/null
    ret=$?
    if [ $ret -ne 0 ]; then
        echo "dataLogDir=${data_log_dir}" >> $install_path/conf/zoo.cfg
        ret=$?
    else
        sed -i "s%.*dataLogDir=.*%dataLogDir=${data_log_dir}%g" $install_path/conf/zoo.cfg
        ret=$?
    fi
    if [ $ret -eq 0 ]; then
        echo "config dataLogDir success"
    else
        echo "config dataLogDir fail"
    fi


    #touch myid
    myid=`echo $name | awk -F "_" '{split($NF, a, "-"); if (a[1]=="myid") print a[2]; else print "X" }'`
    echo $myid > $data_dir/myid
    ret=$?
    if [ $ret -eq 0 ]; then
        echo "touch myid: '${myid}' success"
    else
        echo "touch myid: '${myid}' fail"
    fi
fi

if [ -f $install_path/conf/jmxremote.password ]; then
    chmod 600 $install_path/conf/jmxremote.password
    ln -s $install_path/conf/jmxremote.password $install_path/jdk1.8.0_65/jre/lib/management/jmxremote.password
    if [ $? -eq 0 ]; then
        echo "ln '${install_path}/jmxremote.password' succ"
    else
        echo "ln '${install_path}/jmxremote.password' fail"
    fi
fi
