#!/bin/sh

etc_path=/data/services/qzone_adsdev/etc/
cd $etc_path

ads_ini=$etc_path/ads.ini
ads_bin=$etc_path/ads.bin

ep_tool=$etc_path/ep_tools.conf
fanli=$etc_path/fanli_shop.conf

ttc_ep_bin=$etc_path/ttc_ep.bin
ttc_ep_ini=$etc_path/ttc_ep.ini

ipfile="ep.iplist"
if [ ! -f $ipfile ]
then
    echo "no file"
    exit
fi
sed 's/[ \t^M]*//g' $ipfile| sed -e '/^$/d' > ep.iplist.tmp

ipfile="ep.iplist.tmp"

#更新bin
chmod a+x config_tools
./config_tools $ads_ini $ads_bin
./config_tools $ttc_ep_ini $ttc_ep_bin

while read ip
do
    echo $ip
	expect -f $etc_path/ep_scp.expect $ads_ini $ip $etc_path
	expect -f $etc_path/ep_scp.expect $ads_bin $ip $etc_path

	expect -f $etc_path/ep_scp.expect $ep_tool $ip $etc_path
	expect -f $etc_path/ep_scp.expect $fanli   $ip $etc_path
	
	expect -f $etc_path/ep_scp.expect $ttc_ep_bin $ip $etc_path
	expect -f $etc_path/ep_scp.expect $ttc_ep_ini $ip $etc_path
done < $ipfile



