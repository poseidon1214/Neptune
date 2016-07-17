#!/bin/sh
PATH=/usr/local/sbin:/usr/sbin:/sbin:/usr/local/bin:/usr/bin:/bin
tmp_file=/tmp/qzone_adsdev_debug.txt
dir=$PATH_QZONE_PRJ/etc/
tool=$dir/config_tools
cd $dir

find $dir -name "*.debug" -type f > $tmp_file 
for file in `cat $tmp_file`
do
	if [ -f $file ]
	then
		path=${file%/*}
		file_name=`echo $file | awk -F/ '{print $NF}'`
		file_pre=`echo $file_name | awk -F. '{print $1}'`
		$tool $file "$path/$file_pre"".bin" 
		#echo $path 
		#echo $file_name
		#echo $file_pre
		#echo "$path/$file_pre"".bin" 
	fi 
done

