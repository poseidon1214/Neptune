#killall -12 spp_exp_search_ctrl
killall -9 spp_exp_search_ctrl
killall -9 spp_exp_search_proxy
killall -9 spp_exp_search_worker

sleep 2

./spp_exp_search_ctrl ../etc/spp_ctrl.xml 
