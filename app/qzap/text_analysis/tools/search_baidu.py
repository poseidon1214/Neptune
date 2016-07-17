#!/usr/bin/env python
# coding=utf-8
# author: Lifeng Wang (fandywang@tencent.com)
# date: 2011-12-31
#
# 爬取百度搜索结果，并持久化存储
# Usage：python search_baidu.py <keyword_file> <output_dir> <log_file>

import commands
import logging
import os
from Queue import Queue
import random
import re
import sys
import time
from common.common import sopen

class SearchEngine:
    """
    爬取搜索引擎结果，用于keyword词典过滤，keyword扩展，SAS语义扩展服务等
    """
    def __init__(self, output_dir, log_file):
        """
        初始化，需要指定log和搜索结果输出目录
        """
        self.logger = logging.getLogger()  # log日志相关设置
        handler = logging.FileHandler(log_file)
        formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
        self.logger.setLevel(logging.NOTSET)

        self.query_to_search_result = {}  # 搜索结果词典
        if not os.path.exists(output_dir):
            os.mkdir(output_dir)
        self.output_dir = output_dir  # 搜索结果输出目录

    def search(self, query_file, enc = 'utf-8'):
        """
        对外接口，以 query 文件为输入
        """
        queue = self.fill_queue(query_file, enc)

        query_num = 0
        while not queue.empty():
            query = queue.get()
            is_ok, search_result = self.get_search_result( \
            query.replace(' ', '+'))

            if is_ok != 0:  # 失败
                queue.put(query)
                self.logger.error('%s error! sleep and try later' % (query))
                time.sleep(random.randint(10, 60))
            else:
                query_num += 1
                self.add_search_result(query, search_result)
                if query_num % 3000 == 0:
                    self.logger.info('Downloading query : %d' % query_num)
                    self.save(enc)
        self.save(enc)  # 注意最后把最后不足3000的结果输出

    def fill_queue(self, query_file, enc='utf-8'):
        """
        将query一次性加载到内存中，使用队列queue数据结构维护
        """
        queue = Queue()
        istream = sopen(query_file, 'r', enc)
        for line in istream:
            tokens = line.rstrip(os.linesep).split('\t')
            query = tokens[0]
            if len(query) > 0:
                queue.put(query)
        return queue

    def get_search_result(self, query):
        """
        使用 wget 爬取百度搜索结果
        """
        url = "http://www.baidu.com/s?wd=" + query.encode("gb2312", "ignore");
        return commands.getstatusoutput('wget -qO- ' + url)

    def add_search_result(self, query, search_result):
        """
        匹配抽取搜索结果：snippet、ads和related search，并保存到
        query_to_search_result 词典中
        """
        result = ''
        for match in re.finditer("(?sim)<table.*?</table>", search_result):
            result = result + match.group()
        # 合并成一行
        result = re.sub(r"(?sim)(^$)|(<style.*?</style>)|(\s{2,})|([\r\n])", \
        " ", result)

        self.query_to_search_result[query] = result

    def save(self, enc):
        """
        将搜索结果输出到文件中，文件名根据当前系统时间自动生成
        每行格式为：query\t\tsearch_result
        """
        filename = self.output_dir + '/' + \
        time.strftime("%Y%m%d%H%M%S", time.localtime()) + '.baidu'

        ostream = sopen(filename, 'wb', enc)
        for query, search_result in self.query_to_search_result.items():
            ostream.write(\
            (query + '\t\t' + search_result.decode('utf-8', 'ignore') + '\n'))
        ostream.close()
        self.query_to_search_result = {}  # 清空词典

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print "Usage : python search_baidu.py <keyword_file> <output_dir> \
        <log_file>"
        sys.exit(1)

    se = SearchEngine(sys.argv[2], sys.argv[3])
    se.search(sys.argv[1])
