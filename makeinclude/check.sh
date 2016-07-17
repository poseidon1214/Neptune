#!/bin/sh

if [ $# -ne 2 ];then
	echo "$0 filename funcpatch"
	exit
fi
DOCPATH=$1

#echo "danger check: $DOCPATH"

while read FUNC tail
do
	#echo "----------find $FUNC ---------------------------------"
	temp=`grep -R -n --include=\*.{c,cpp,h,hpp,C,cxx,cc} "\b$FUNC\>" $DOCPATH | grep -v "banned_tx.h" `
	count=`echo $temp | grep ":" | wc -l`
	if [ $count -gt 0 ];then
		echo "$temp"
		exit 1
	fi
done < $2

