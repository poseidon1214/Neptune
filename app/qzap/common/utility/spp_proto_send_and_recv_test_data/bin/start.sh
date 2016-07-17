echo `basename $0`
echo `dirname $0`
cd `dirname $0`
ulimit -c unlimited
n=`ps -ef|grep spp_proto_send_and_recv_ctrl|grep -v qzap | grep -v grep|wc -l`
if [ $n -lt 1 ]
then	
./spp_proto_send_and_recv_ctrl ../conf/spp_ctrl.xml 
fi
