#!/usr/bin/env python
# -*- coding: utf-8 -*-
#----------------------------------------------------------------------------#
# util.py
# Zhihui Jin <zhihuijin@gmail.com>
# vim: ts=4 sw=4 sts=4 et tw=78:
# Tue Sep  2 14:02:41 CST 2008
#
#----------------------------------------------------------------------------#

""" 一些用于维护分词词典的公用函数
    
"""

#----------------------------------------------------------------------------#

import sys, os, glob, optparse, codecs
import hanziTable
from common import sopen, filterComments
from table import parseLines
from zhenc  import is_alphabet, is_chinese

# 繁体-简体映射
_relation    = hanziTable.getCognateRelation()
_chs_map_dic = _relation.reverseMap()
_cht_set     = set(_chs_map_dic.keys())

# GB2312 中的24个繁体字的繁体-简体映射
_relation_gb    = hanziTable.getGB2312CognateRelation()
_chsgb_map_dic  = _relation_gb.reverseMap()
_chtgb_set      = set(_chsgb_map_dic.keys())


#----------------------------------------------------------------------------#
def uprint (mesg, stream = sys.stdout, enc = sys.stdin.encoding):
    """ 按指定的编码打印
    """
    if enc is None:
        enc = 'gb18030'
    print>>stream, mesg.encode(enc)

#----------------------------------------------------------------------------#
def is_chinese_str(str):
    """ 检测是否为全汉字串
    """
    for uchar in str:
        if not is_chinese(uchar):
            return False

    return True

#----------------------------------------------------------------------------#
def is_alphabet_str(str):
    """ 检测是否为全汉字串
    """
    for uchar in str:
        if not is_alphabet(uchar):
            return False

    return True

#----------------------------------------------------------------------------#
def has_cht(word):
    """ 判断是否含有繁体字
    """
    for ch in word:
        if ch in _cht_set:
            return True
    return False

#----------------------------------------------------------------------------#
def has_gbcht(word):
    """ 判断是否含有繁体字
    """
    for ch in word:
        if ch in _chtgb_set:
            return True
    return False

#----------------------------------------------------------------------------#
def cht2chs(word, conv_gbcht=False):
    """ 繁体词条转简体
    """
    result = ''
    for ch in word:
        if ch in _cht_set:
            chs = ''.join(list(_chs_map_dic[ch]))
            if not conv_gbcht and ch in _chtgb_set:
                result += ch
            else:
                result += chs
        else:
            result += ch

    return result

#----------------------------------------------------------------------------#
def has_alphabet(word):
	""" 是否有英文字母
	"""
	for c in word:
		if is_alphabet(c):
			return True
	return False

#----------------------------------------------------------------------------#
def write_list(line_list, fname, enc):
    """ 把列表写入文件
    """
    ostream = sopen(fname, 'wb', enc)
    for line in line_list:
        print >> ostream, line
    ostream.close()


#----------------------------------------------------------------------------#
def strip_html(html_page):
    """ 去除 html tag
    """
    cmd = 'lynx -stdin -raw -dump -nolist' 
    pipe_in, pipe_out= os.popen2(cmd, 'wr')
    stream_in  =  codecs.getwriter('utf-8')(pipe_in,  'ignore')
    stream_out =  codecs.getreader('utf-8')(pipe_out, 'ignore')
    print >> stream_in, html_page
    stream_in.close()

    content = stream_out.read()
    stream_out.close()
    return content

#----------------------------------------------------------------------------#
def readList(filename):
    """ return the character list in file
    """
    list = [line.rstrip() for line in filterComments(sopen(filename))]
    return list

#----------------------------------------------------------------------------#
def writeList(outList, filename):
    """ write the list to file
    """
    outFile = sopen(filename, 'w')
    for word in outList:
        print>>outFile, word
    outFile.close()

#----------------------------------------------------------------------------#
def readDict(filename, sep=' '):
    """ read the dict from file
    """
    dictObj = {}
    for key, value in parseLines(sopen(filename), sep, n=2):
        dictObj[key] = value

    return dictObj

#----------------------------------------------------------------------------#
def writeDict(outDict, filename, sep=' '):
    """ write the dict to file
    """
    outFile = sopen(filename)
    for word in outDict.keys():
        print>>outFile, word + sep + outDict[word]
    outFile.close()

#----------------------------------------------------------------------------#
def commList(firstList, secondList):
    """ return the intersection of two list
    """
    commSet = set(firstList).intersection(set(secondList))
    return list(commSet)

#----------------------------------------------------------------------------#
def prodList(firstList, secondList):
    """ return the Cartesian product of two list
    """
    if len(firstList) == 0:
        return secondList
    if len(secondList) == 0:
        return firstList

    result = []
    for a in firstList:
        for b in secondList:
            result.append(a + b)

    return result

#----------------------------------------------------------------------------#
def shrink_counter(counter, portion, delta, freq_key=lambda x : x):
    """ 词典太大的时候， 裁减低频部分
        @portion 裁减后的大小不能超过 原始大小 * portion
        @delta   去除频率低于 delta 的项
        @param freq_key 键值函数,把每个 entry 映射为对应的频率

        裁减策略如下：
        a. 对于 (word, value) pair， 使用 freq_key(value) 获取到频率 freq
           裁减 freq < delta 的 item
        b. 把余下的按 freq 降序排列， 只保留 top-N 个( N=原始大小 * portion)
    """
    old_size = len(counter)
    new_size = int(old_size * portion)
    #print '.... shrink counter %s --> %s ' %(old_size, new_size)
    for word, val in counter.items():
        if freq_key(val) < delta:
            counter.pop(word)

    sort_key = lambda x : freq_key(x[1])
    if len(counter) > new_size:
        items = counter.items()
        items.sort(key=sort_key, reverse=True)
        for i in range(new_size, len(counter)):
            word = items[i][0]
            counter.pop(word)

#------------------------------------------------------------------------#
