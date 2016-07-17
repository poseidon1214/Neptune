#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Tencent Inc.
# All rights reserved.
#
# Author: Chen Feng <phongchen@tencent.com>
#
# Run cppcheck on given files with gdt code organization.

import os
import subprocess
import sys

import common


def cppcheck(args):
    cppcheck_bin = os.path.join(os.path.dirname(__file__), 'cppcheck')
    options = [
        '--enable=warning,style,performance,portability,information,missingInclude',
        '--template="{file}:{line}: {severity}: {id}: {message}"',
        '--inline-suppr',
        '--suppress=class_X_Y',
        '--suppress=toomanyconfigs',
        '--suppress=unusedPrivateFunction',
        '--suppress=unmatchedSuppression',
        '--suppress=noConstructor:*app/qzap/common/base/callback_impl.h',
        '--suppress=stlSize:*app/qzap/common/cpp/include/wup_qzap_proto_search_broker.h',
        '--suppress=*:*thirdparty/*.*',
        '-DGTEST_HAS_PARAM_TEST=1',
        '-DGTEST_HAS_DEATH_TEST=1',
        '-DGTEST_HAS_STD_WSTRING=1',
        '-DGTEST_HAS_DEATH_TEST=1',
        '-UGTEST_OS_WINDOWS',
        '-U_MSC_VER',
        '-U_WIN32',
        '-Umax',
        '-Umin',
        '-D__GNUC__=4',
        '-D__x86_64',
        '-D__i386__',
        '-j4',
        '-I %s' % common.find_blade_root_dir()
    ]
    cmd = "%s %s %s" % (cppcheck_bin, ' '.join(options), ' '.join(args))
    return subprocess.call(cmd, shell=True)


def __main__():
    filenames = sys.argv[1:]
    cppcheck(filenames)


if __name__ == '__main__':
    __main__()

