#!/usr/bin/env python
# -*- coding: utf-8 -*-
#----------------------------------------------------------------------------#
# path_walk.py
# Zhihui Jin <rickjin@tencent.com>
# vim: ts=4 sw=4 sts=4 et tw=78:
# Tue Sep  2 14:02:41 CST 2008
#
#----------------------------------------------------------------------------#

""" 列出给定路径下的指定后缀文件
"""

#----------------------------------------------------------------------------#

import sys, os
from util import uprint
from tools.common import sopen

#------------------------------------------------------------------------#
def normalize_path(path_str):
    """ 把路径处理为 windows/linux 系统对应的形式
    """
    path_elements = [path_str]
    if '/' in path_str:
        path_elements = path_str.split('/')
    elif '\\' in path_str:
        path_elements = path_str.split('\\')

    path = os.path.sep.join(path_elements)
    return os.path.normpath(path)

#----------------------------------------------------------------------------#
def parse_config_file(conf_file, enc):
    """ 解析配置文件，获取inlucde /exclude 路径列表
        
        配置文件中使用 
        Include <path>  来指明需要扫描的路径
        Exclude <path>  用于指明需要去除的子路径

        eg:
        Include words
        Exclude words/config
        Exclude words/delete

    """
    include_list = []
    exclude_list = []
    for line in sopen(conf_file, 'r', enc):
        line = line.strip()
        if len(line)== 0 or  line[0] == '#':
            continue

        fields = line.split()
        if len(fields) != 2:
            uprint('Skip error in config file : %s' %line, sys.stderr)

        if fields[0].lower() == 'include':
            path = normalize_path(fields[1])
            include_list.append(path)
        elif fields[0].lower() == 'exclude':
            path = normalize_path(fields[1])
            exclude_list.append(path)
        else:
            uprint('Skip error in config file : %s' %line, sys.stderr)

    return include_list, exclude_list

#----------------------------------------------------------------------------#
def traverse_path(ext, in_path, exclude_list = []):
    """ 遍历路径， 返回该路径下的以 ext 为扩展名的文件

    @param ext     需要收集的文件扩展名, 如果为空串 '', 则返回所有文件
    @param in_path 检索路径
    @param exclude_list 需要去除的子路径, 可以为相对路径或绝对路径
    """

    # 路径绝对化
    in_path = os.path.abspath(in_path)
    exclude_list = [os.path.abspath(path) for path in exclude_list]

    file_list = []
    if os.path.isfile(in_path):
        if in_path.endswith(ext) and in_path not in exclude_list:
            file_list.append(in_path)
        return file_list

    for subdir in os.listdir(in_path):
        subpath = os.path.join(in_path, subdir)
        if subpath in exclude_list:
            continue

        if os.path.isfile(subpath) and subpath.endswith(ext):
            file_list.append(subpath)
        else:
            file_list += traverse_path(ext, subpath, exclude_list)

    return file_list


#----------------------------------------------------------------------------#
def traverse_path2(ext, include_list, exclude_list = []):
    """ 获取文件列表
    @param ext          文件扩展名
    @param include_list 包含的路径
    @param exclude_list 去除的路径
    """
    file_list = []
    for in_path in include_list:
        cur_list = traverse_path(ext, in_path, exclude_list)
        for file in cur_list:
            if file not in file_list:
                file_list.append(file)

    return file_list

#----------------------------------------------------------------------------#
def traverse_path3(ext, in_path_str, exclude_list = []):
    """ 获取文件列表
    @param ext          文件扩展名
    @param in_path_str  多个路径用 os.path.pathsep 分隔
                        windows 下为 ; linux 下为 :
    @param exclude_list 去除的路径
    """
    file_list = traverse_path2(ext, in_path_str.split(os.path.pathsep), exclude_list)
    return file_list

#----------------------------------------------------------------------------#
def traverse_path4(ext, conf_file, enc):
    """ 
    通过解析配置文件获取需要的文件列表

    @param ext       文件扩展名
    @param conf_file 路径选取配置文件
    @param enc       配置文件编码
    """
    include_list, exclude_list = parse_config_file(conf_file, enc)
    file_list = traverse_path2(ext, include_list, exclude_list)
    return  file_list

#----------------------------------------------------------------------------#
# vim: ts=4 sw=4 sts=4 et tw=78:
