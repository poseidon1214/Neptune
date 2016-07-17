SHM_IDS=`ipcs -m | awk '{if (match($1, /0x008001/)) print $1}'`
for SHM_ID in $SHM_IDS 
do
#	echo "ipcrm -M $SHM_ID"
	ipcrm -M $SHM_ID 
done

MQ_IDS=`ipcs -q | awk '{if (match($1, /0x008001/)) print $1}'`
for MQ_ID in $MQ_IDS 
do
#	echo "ipcrm -Q $MQ_ID"
	ipcrm -Q $MQ_ID 
done
