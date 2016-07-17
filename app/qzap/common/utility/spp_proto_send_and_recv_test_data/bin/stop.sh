cd `dirname $0`
killall -10 spp_proto_send_and_recv_ctrl
killall -9 spp_proto_send_and_recv_worker
killall -9 spp_proto_send_and_recv__proxy
sleep 1
./clear_ipc.sh
