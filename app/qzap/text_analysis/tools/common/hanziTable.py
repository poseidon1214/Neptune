# -*- coding: utf-8 -*-
#----------------------------------------------------------------------------#
# hanziTable.py
# Zhihui Jin <rickjin@tencent.com>
# vim: ts=4 sw=4 sts=4 et tw=78:
# Wed Sep 24 11:51:44 CST 2008
#----------------------------------------------------------------------------#

""" 处理中文简体和繁体之间的字符映射
"""

#----------------------------------------------------------------------------#

import sys, os
from common import sopen
from relation import Relation, ScoredRelation
from table import parseLines

root_dir  = os.path.join(os.path.abspath(os.path.dirname(__file__)), '..') 
sys.path.append(root_dir)
settings = __import__('settings')
del sys.path[-1]

#----------------------------------------------------------------------------#

# Relevant data files.
_tableDir = os.path.join(settings.PROJECT_CONFIG_DIR, 'chres', 'tables')
_cognateTable = os.path.join(_tableDir, 'chs_cht_cognates')
_GB2312CognateTable = os.path.join(_tableDir, 'gb2312_chs_cht_cognates')

#----------------------------------------------------------------------------#
# PUBLIC
#----------------------------------------------------------------------------#

def getCognateRelation():
    """ 读取完整的简繁转换对照表
    """
    return getRelation(_cognateTable, 'chs', 'cht')

#----------------------------------------------------------------------------#
def getGB2312CognateRelation():
    """ 读取GB2312字集中的简繁转换对照表 
    """
    return getRelation(_GB2312CognateTable, 'chs', 'cht')


#----------------------------------------------------------------------------#
def getRelation(table, fromName, toName):
    """ 读取关系对照表
    """
    relation = Relation(fromName, toName)
    for line in sopen(table):
        if line.startswith('#'):
            continue

        flds = line.rstrip().split()
        chs = flds[0]
        for cht in flds[1:]:
            relation.add(chs, cht)

    return relation

#----------------------------------------------------------------------------#

