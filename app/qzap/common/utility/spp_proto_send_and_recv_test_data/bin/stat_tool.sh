#!/bin/sh
RED=\\e[1m\\e[31m
DARKRED=\\e[31m 
GREEN=\\e[1m\\e[32m
DARKGREEN=\\e[32m 
BLUE=\\e[1m\\e[34m
DARKBLUE=\\e[34m 
YELLOW=\\e[1m\\e[33m
DARKYELLOW=\\e[33m 
MAGENTA=\\e[1m\\e[35m
DARKMAGENTA=\\e[35m 
CYAN=\\e[1m\\e[36m
DARKCYAN=\\e[36m 
RESET=\\e[m

if [ $# != 1 ] 
then
	echo -e "$RED USAGE: $0 $YELLOW ./spp_worker.dat$RESET"
	exit 0;
fi

if [  -s $1  ] 
then
	echo ""
else
	echo -e  "stat_file[$RED$1$RESET] does not exist!"
	exit 0;
fi

I=0
echo -e "$DARKGREEN MAX_PROC_TIME	AVG_PROC_TIME	REQ/S$RESET"
echo -e "--------------------------------------"
while [ 1 ]
do
	I=$(($I+1));
	L=`./stat_tool $1 | grep ator_send_num | awk -F"|"  '{printf $4}' | awk -F" " '{printf $1}'`
	sleep 1
	N=`./stat_tool $1 | grep ator_send_num | awk -F"|"  '{printf $4}' | awk -F" " '{printf $1}'`
	M=`./stat_tool $1 | grep max_proc_time | awk -F"|"  '{printf $5}' | awk -F" " '{printf $1}'`
	A=`./stat_tool $1 | grep avg_proc_time | awk -F"|"  '{printf $5}' | awk -F" " '{printf $1}'`
	REQ=$(($N-$L))

	if [ $REQ -lt 0 ]
	then
		sleep 2
		continue
	fi

	if [ $(($I%4)) -eq 0 ] || [ $(($I%4)) -eq 3 ]	
	then
		echo -e "$YELLOW $M		$A		$REQ$RESET"
	else
		echo -e "$MAGENTA $M		$A		$REQ$RESET"
	fi

	if [ $(($I%30)) -eq 0 ]	
	then
		echo -e "$DARKGREEN MAX_PROC_TIME	AVG_PROC_TIME	REQ/S$RESET"
		echo -e "--------------------------------------"
	fi

done
