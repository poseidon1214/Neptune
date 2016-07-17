#!/usr/bin/env python
# coding=utf-8
# author: Lifeng Wang (fandywang@tencent.com)
# date: 2012-01-01
#
# 解析爬取的百度搜索结果，并持久化存储
# "Usage : python search_result_extractor.py <type>
#                 <search_result_dir> <output_dir> <log_file>

import logging
import os, sys, time
import re
from common.common import sopen

class SearchResultExtractor:
    """
    解析搜索引擎检索结果，获取关键文本：广告、snippet和相关搜索
    """
    def __init__(self, search_result_dir, output_dir, log_file, enc = 'utf-8'):
        """
        初始化，需要指定log和搜索结果输出目录
        """
        self.logger = logging.getLogger()  # log日志相关设置
        handler = logging.FileHandler(log_file)
        formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
        self.logger.setLevel(logging.NOTSET)

        if not os.path.exists(output_dir):
            os.mkdir(output_dir)
        self.output_dir = output_dir  # 搜索结果输出目录

        self.query_to_search_result = {}  # 搜索结果词典
        self.search_result_dir = search_result_dir

    def extract(self, suffix, enc = 'utf-8'):
        """
        对外调用接口
        """
        query_num = 0
        for root, dirs, files in os.walk(self.search_result_dir):
            for file in files:
                self.logger.info('Processing file : ' + file)

                istream = sopen(os.path.join(root, file), 'rb', 'byte')
                for line in istream:
                    line_enc = unicode(line, 'utf-8', 'replace')
                    items = line_enc.split('\t\t', 1)
                    if len(items) < 2:
                        self.logger.warning('The format of line : ' + line \
                        + ' is illegal.')
                    else:
                        if cmp(suffix, 'ads') == 0:
                            self.extract_ads(items[0], items[1])
                        elif cmp(suffix, 'snippet') == 0:
                            self.extract_snippet(items[0], items[1])
                        elif cmp(suffix, 'related_search') == 0:
                            self.extract_related_search(items[0], items[1])
                        else:
                            print 'The "suffix" parameter must be "ads",\
                            "snippet", and "related_search"'
                            sys.exit(1)

                        query_num += 1
                        if query_num % 10000 == 0:
                            self.save(suffix, enc)
                istream.close()
        self.save(suffix, enc)  # 注意保存最后一组数据

    def extract_ads(self, query, search_result):
        """
        format: query\t\tad_1(title@@@summary)\tad_2\t...
        """
        ads = ''
        # 右栏广告
        for match in re.finditer(r'(?sim)<div id="bdfs\d+" class="EC_fr EC_PP".*?><a .*? target="_blank"><font size="3">(.*?)</font></a><br><a href=.*?><font size="-1" color="#000000">(.*?)</font><br><font size="-1" color="#008000">', search_result):
            ads += match.group(1) + '@@@' + match.group(2) + '\t'
        # 左上、左下栏广告
        for match in re.finditer(r'(?sim)<tr>\s+<td [^>]*?style="line-height:24px;" class="f16 EC_PP" id="taw\d">\s+<a target="_blank" [^>]*?>\s+<font size="3" style="text-decoration:underline;">(.*?)</font><font size="-1" color="#008000".*?</a>\s+</td>.*?</tr>\s+<tr>\s*<td .*? id="taw\d" class="f16 EC_PP".*?>.*?<font size="-1">(.*?)</font></a>\s+</td>\s+</tr>', search_result):
            ads += match.group(1) + '@@@' + match.group(2) + '\t'
        # 搜索结果页中植入广告(带推广字样)
        for match in re.finditer(r'(?sim)<table id="\d+" .*?class="ec_pp_f" border="0" cellpadding="0" cellspacing="0"><tr><td class="f EC_PP"><a id="aw\d+" [^>]*? target="_blank"><font size="3">(.*?)</font></a><br>\s+<font size=-1>(.*?)</font><br>', \
                        search_result):
            ads += match.group(1) + '@@@' + match_group(2) + '\t'
        self.query_to_search_result[query] = ads

    def extract_snippet(self, query, search_result):
        """
        format: query\t\tsnippet_1(title@@@summary)\tsnippet_2\t...
        """
        snippets = ''
        for match in re.finditer('(?sim)<td class=f><h3 class="t"><a onmousedown=.*?target="_blank">((.*?)</a> </h3><font size=-1>(.*?))<br><span class="g">.*?</font>', search_result):
            snippets += re.sub("(?sim)</a> </h3><font size=-1>", "@@@", \
            match.group(1)) + '\t'
        self.query_to_search_result[query] = snippets

    def extract_related_search(self, query, search_result):
        """
        format: query\t\trelated_search_1\trelated_search_2\t...
        """
        match = re.search('<table cellpadding="0"><tr><th rowspan="2" class="tt">[^<]*?</th>(.*?)</table>', search_result, \
        re.DOTALL | re.IGNORECASE | re.MULTILINE)
        related_searchs = ''
        if match:
            related_search_block = match.group(1)
            for sub_match in re.finditer(r'(?sim)<th><a href="s\?wd=.*?">(.*?)</a>', related_search_block):
                related_searchs += sub_match.group(1) + '\t'
        self.query_to_search_result[query] = related_searchs

    def save(self, suffix, enc):
        """
        将搜索结果输出到文件中，文件名根据当前系统时间自动生成
        每行格式为：query\t\tsearch_result
        """
        filename = self.output_dir + '/' + \
        time.strftime("%Y%m%d%H%M%S", time.localtime()) + '.baidu.' + suffix

        ostream = sopen(filename, 'wb', enc)
        for query, search_result in self.query_to_search_result.items():
            ostream.write(\
            (query + '\t\t' + search_result + '\n'))
        ostream.close()
        self.query_to_search_result = {}  # 清空词典

if __name__ == '__main__':
    if len(sys.argv) != 5:
        print "Usage : python search_result_extractor.py <type> \
        <search_result_dir> <output_dir> <log_file>"
        sys.exit(1)

    sr = SearchResultExtractor(sys.argv[2], sys.argv[3], sys.argv[4])
    sr.extract(sys.argv[1])
