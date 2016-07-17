#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Tencent Inc.
# All rights reserved.
#
# Author: Chen Feng <phongchen@tencent.com>
#
# Run astyle on given files with gdt code organization.

import os
import subprocess
import sys

import common


def astyle(files):
    curdir = os.path.dirname(__file__)
    astyle_bin = os.path.join(curdir, 'astyle')
    options_file = os.path.join(curdir, 'astylerc')
    subprocess.call("%s --options=%s %s" % (astyle_bin,
            options_file, ' '.join(files)), shell=True)


def __main__():
    filenames = sys.argv[1:]
    astyle(filenames)


if __name__ == '__main__':
    __main__()

