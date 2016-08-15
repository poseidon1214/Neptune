#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2015 Tencent Inc.
# Author: Huan Yu <huanyu@tencent.com>

"CIS test"

from subprocess import Popen, PIPE
import csv
import urllib
import json
import time
import sys
import xml.etree.ElementTree as ET

sys.defaultencoding = 'utf8'

output = Popen(("svn log -q -v --xml -r {%s}:{%s}" % (sys.argv[1], sys.argv[2])).split(" "), stdout=PIPE).communicate()[0]

path_map = [
    ('/qzone_adsdev_proj/trunk/adx', '基础功能组'),
    ('/qzone_adsdev_proj/trunk/app/brand', '业务功能组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/common', 'common'),
    ('/qzone_adsdev_proj/trunk/app/qzap/log_process', '日志平台组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/proto', 'common'),
    ('/qzone_adsdev_proj/trunk/app/qzap/proto/db', '基础平台组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/account', '基础平台组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/ad_portrait', '广告定向组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/adbill', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/adbill_new', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/adbroker2', '播放系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/adclick', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/adconv', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/addispcache', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/addisplay', '播放系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/addisplay/rtb_cgi', '基础功能组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/addispreport', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/adinfockv', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/adrealrpt', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/adstrategy', '广告定向组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/advertiser_datamining', '优化智能组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/anti_spam', '反作弊组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/auth', '基础平台组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/billing_center', '基础平台组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/brand_ad', '创意优化组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/cookie_mapping', '播放系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/cvr', '转化优化组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/data_push_pipeline', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/datainterface', '基础平台组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/datamigration', '基础平台组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/wavelet', '创意优化组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/feature_server_v3', '创意优化组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/ldpub', '基础平台组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/lite_ctr', '检索策略组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/log_proccess', '日志平台组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/market_task', '业务功能组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/mixer', '播放系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/new_ad', '检索策略组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/query_analysis', '创意优化组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/remkt_rule_pre_calc', '广告定向组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/reranking', '竞价排序组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/retrieval', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/scoring2_offline', '检索策略组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/scoring3', '检索策略组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/scoring_pipeline', '检索策略组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/search_ctr', '创意优化组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/site_updater', '播放系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/sns_interaction', '播放系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/static_service', '基础平台组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/strategy_data_platform', '竞价排序组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/sunfish', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/targeting', '广告定向组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/tips_trigger', '播放系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/traffic_control', '检索策略组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/service/training2', '检索策略组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/sites/c.gdt.qq.com', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/sites/cm.e.qq.com', '播放系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/sites/i.e.qq.com', '基础功能组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/sites/i.gdt.qq.com', '播放系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/sites/mi.e.qq.com', '基础功能组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/sites/mi.gdt.qq.com', '播放系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/sites/q.i.gdt.qq.com', '播放系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/sites/sns.gdt.qq.com', '业务功能组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/sites/t.gdt.qq.com', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/sites/v.gdt.qq.com', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/sites/yk.i.gdt.qq.com', '基础功能组'),
    ('/qzone_adsdev_proj/trunk/app/qzap/text_analysis', '广告定向组'),
    ('/qzone_adsdev_proj/trunk/app/tubi', '广告定向组'),
    ('/qzone_adsdev_proj/trunk/base_class', 'common'),
    ('/qzone_adsdev_proj/trunk/base_class_old', 'common'),
    ('/qzone_adsdev_proj/trunk/btsync', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/common', '业务功能组'),
    ('/qzone_adsdev_proj/trunk/components', 'common'),
    ('/qzone_adsdev_proj/trunk/creative', '创意优化组'),
    ('/qzone_adsdev_proj/trunk/display', '播放系统组'),
    ('/qzone_adsdev_proj/trunk/dmp', '广告定向组'),
    ('/qzone_adsdev_proj/trunk/feeds', '业务功能组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adnetwork/adnetwork-api', '投放工具组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adnetwork/adnetwork-oss', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adnetwork/adnetwork-service', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adnetwork/adnetwork-task', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adnetwork/adnetwork-web', '投放工具组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adnetwork/adnetwork_wechat', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adnetwork/adnetwork_wechat/adnetwork_devnew', '投放工具组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adnetwork/api-proxy', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adnetwork/mobsdk', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adnetwork/tg-cMVP', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adnetwork/tg-widgets', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adnetwork/webDev', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adx', '运营平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adx/dmp-api', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adx/dmp-service', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adx/dmp-web', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adx/dsp-service', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adx/dsposs', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adx/dsposs', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adx/java-di-invoice', 'common'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/adx/java-di-invoice-config', 'common'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/gdcpm', '投放工具组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/incubator', '投放工具组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/infra', 'common'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/msp', '投放工具组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/oss', '运营平台组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/public', 'common'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/tools', '投放工具组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/venus', '投放工具组'),
    ('/qzone_adsdev_proj/trunk/java/BizRd/webDev-api', '媒介平台组'),
    ('/qzone_adsdev_proj/trunk/java/antispam', '反作弊组'),
    ('/qzone_adsdev_proj/trunk/java/com/gdt/marvel', '日志平台组',),
    ('/qzone_adsdev_proj/trunk/java/com/gdt/monitor', '日志平台组'),
    ('/qzone_adsdev_proj/trunk/java/com/gdt/recordio', 'common',),
    ('/qzone_adsdev_proj/trunk/java/com/gdt/zk_config', '日志平台组'),
    ('/qzone_adsdev_proj/trunk/java/cvr', '转化优化组'),
    ('/qzone_adsdev_proj/trunk/java/dmp', '广告定向组'),
    ('/qzone_adsdev_proj/trunk/java/gdt_log_process', '日志平台组'),
    ('/qzone_adsdev_proj/trunk/java/recordio', 'common',),
    ('/qzone_adsdev_proj/trunk/java/sstable', 'common',),
    ('/qzone_adsdev_proj/trunk/java/sunfish', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/java/targeting', '广告定向组'),
    ('/qzone_adsdev_proj/trunk/java/thirdparty', 'common'),
    ('/qzone_adsdev_proj/trunk/logflow', '检索系统组',),
    ('/qzone_adsdev_proj/trunk/marvel', '日志平台组',),
    ('/qzone_adsdev_proj/trunk/resource/qzap/data/addisplay', '播放系统组'),
    ('/qzone_adsdev_proj/trunk/resource/qzap/data/click', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/resource/qzap/data/conv', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/resource/qzap/data/market_task', '业务功能组'),
    ('/qzone_adsdev_proj/trunk/sunfish', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/supermerger', '检索系统组'),
    ('/qzone_adsdev_proj/trunk/thirdparty', 'common'),
    ('/qzone_adsdev_proj/trunk/wechat', 'Wechat'),
]

root = ET.fromstring(output)

author_group_map = {}

group_map = {}

def get_group(user_id):
    if user_id in group_map:
        return group_map[user_id]
    url = 'http://testing.sng.local/user_info.php?username=' + user_id
    for i in xrange(10):
        try:
            res = urllib.urlopen(url)
            str = res.read()
            res.close()
            break
        except:
            time.sleep(1)
            pass
    if not str:
        print 'Cannot get group info for: %s' % user_id
        sys.exit(-1)
    name_dic = json.loads(str)
    if name_dic:
        if name_dic[user_id]['group']:
            group_map[user_id] = name_dic[user_id]['group']
        if name_dic[user_id]['depart'] != u"社交与效果广告部":
            group_map[user_id] = "Unknown"
    if not user_id in group_map:
        group_map[user_id] = "Unknown"
    return group_map[user_id]

for child in root:
    # print child.attrib
    for author_node in child.iter('author'):
        author = author_node.text
        author = get_group(author)
        p_map = author_group_map.get(author, {})
        found = set()
        for p_node in child.iter('path'):
            path = p_node.text
            length = 0
            match = ''
            for p in path_map:
                if path.startswith(p[0]):
                    if len(p[0]) > length:
                        length = len(p[0])
                        match = p[1]
            # Debug code
            # if match == '投放工具组' and author == u'媒介平台组':
            #     print author_node.text
            #     print path
            if length:
                found.add(match)
            else:
                print path.encode('utf-8')
                pass
        for i in found:
            p_map[i] = p_map.get(i, 0) + 1
        author_group_map[author] = p_map

with open(sys.argv[3], 'w') as csvfile:
    fieldnames = ['id'] + [j.encode('gbk') for j in sorted(list(set([i[1].decode('utf8') for i in path_map])))]
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

    writer.writeheader()
    for author in sorted(author_group_map.keys()):
        z = {'id': author.encode('gbk')}
        for k, v in author_group_map[author].items():
            z[k.decode('utf8').encode('gbk')] = v
        writer.writerow(z)
