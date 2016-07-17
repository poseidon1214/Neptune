# -*- coding: utf-8 -*-
#----------------------------------------------------------------------------#
# ZhihuiJin <zhihuijin@gmail.com>
# vim: ts=4 sw=4 sts=4 et tw=78:
# Fri Dec  8 14:46:39 2006
#
#----------------------------------------------------------------------------#

""" 处理中文字符的接口
"""

#----------------------------------------------------------------------------#

import os, sys, re
import hanziTable
from util import readList, commList

root_dir  = os.path.join(os.path.abspath(os.path.dirname(__file__)), '..') 
sys.path.append(root_dir)
settings = __import__('settings')
del sys.path[-1]

#----------------------------------------------------------------------------#

_charListDir = os.path.join(settings.PROJECT_CONFIG_DIR, 'chres', 'lists', 'char')

ch_gb2312               = os.path.join(_charListDir, 'ch_gb2312')
ch_gb2312_level1        = os.path.join(_charListDir, 'ch_gb2312_level1')
ch_common_char_level1   = os.path.join(_charListDir, 'ch_common_char_level1')
ch_common_char_level2   = os.path.join(_charListDir, 'ch_common_char_level2')

#----------------------------------------------------------------------------#
def getGB2312():
    """ get GB2312 charset
    """
    return readList(ch_gb2312)

#----------------------------------------------------------------------------#
def getGB2312Level1():
    """ get GB2312 charset, level-1
    """
    return readList(ch_gb2312_level1)

#----------------------------------------------------------------------------#
def getChCommonLevel1():
    """ get commonly used hanzi, level 1
    """
    return readList(ch_common_char_level1)

#----------------------------------------------------------------------------#
def getChCommonLevel2():
    """ get commonly used hanzi, level 2
    """
    return readList(ch_common_char_level2)

#----------------------------------------------------------------------------#
def getChCommonChar():
    """ get commonly used hanzi set, including level-1 and level-2
    """
    list =  getChCommonLevel1()
    list +=  getChCommonLevel2()
    return list


#----------------------------------------------------------------------------#
def chs2cht(chsSet):
    """ Get traditional hanzi for simplified hanzi 
    """
    relation    = hanziTable.getCognateRelation()
    chtMapDic = relation.forwardMap()
    chtSet    = set()
    for chs in commList(chtMapDic.keys(), chsSet):
        for chs in chtMapDic[chs]:
            chtSet.add(chs)

    return list(chtSet)

#----------------------------------------------------------------------------#
def cht2chs(chtSet):
    """ Get simplified hanzi for traditional hanzi 
    """
    relation  = hanziTable.getCognateRelation()
    chsMapDic = relation.reverseMap()
    chsSet    = set()
    for cht in commList(chsMapDic.keys(), chtSet):
        for cht in chsMapDic[cht]:
            chsSet.add(cht)

    return list(chsSet)
#----------------------------------------------------------------------------#
