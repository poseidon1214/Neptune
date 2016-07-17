#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, Tencent Inc.
# All rights reserved.
#
# Author: Chen Feng <phongchen@tencent.com>
#
# Run secure scan on c/c++ code


import os
import re
import sys

import common


def _error_exit(msg):
    common.error_exit('Secure Scan Error: %s' % msg)


_EXCEPTION_DIRS = [
    'thirdparty'
]


class SecureScanner(object):
    def __init__(self):
        self.forbidden_fun_regexes = []
        self.init_regex()

    def init_regex(self):
        forbidden_fun_file = "%s/makeinclude/funcs.txt" % common.find_blade_root_dir()
        try:
            file_obj = open(forbidden_fun_file, "r")
        except IOError:
            _error_exit("Please checkout %s first." % forbidden_fun_file)

        match_str = []
        for line in file_obj:
            match_str.append(r"\b%s\b" % line[:-1])
        self.forbidden_fun_regexes = [ re.compile(p) for p in match_str]

    def _check_forbidden_functions_for_file(self, diff_file):
        # Ignore forbidden functions checking if the file does not exist
        # eg. svn rm somefile
        absolute_file_name = os.path.abspath(diff_file.strip())
        try:
            file_object = open(absolute_file_name, "r")
        except IOError:
            return []

        line_number = 0
        forbidden_details = []
        for line in file_object:
            line_number = line_number + 1
            for regex in self.forbidden_fun_regexes:
                match_result = regex.search(line)
                if match_result:
                    comment_start = line.find('//')  # Ignore if in comment
                    if (comment_start == -1 or
                        match_result.start(0) < comment_start):
                        forbidden_details.append(
                            "Forbidden function '%s' in file %s at line:%d" %
                            (regex.pattern[2:-2], diff_file, line_number))
        return forbidden_details

    def filter_out_exceptions(self, files):
        result = []
        blade_root_dir = common.find_blade_root_dir()
        for f in files:
            ff = os.path.normpath(os.path.join(os.environ['PWD'], f))
            for e in _EXCEPTION_DIRS:
                epath = os.path.normpath(os.path.join(blade_root_dir, e))
                if not ff.startswith(epath):
                    result.append(f)
        return result

    def scan(self, files):
        files = self.filter_out_exceptions(files)
        cc_files = common.filter_cc_files(files)
        forbidden_upload_details = []
        for diff_file in cc_files:
            forbidden_upload_details.extend(
                    self._check_forbidden_functions_for_file(diff_file))
        return forbidden_upload_details


def check(files):
    if not files:
        return
    secure_scanner = SecureScanner()
    forbidden_details = secure_scanner.scan(files)
    if forbidden_details:
        _error_exit("".join("\n%s" % p for p in forbidden_details));


def __main__():
    secure_scanner = SecureScanner()
    filenames = sys.argv[1:]
    check(filenames)

if __name__ == '__main__':
    __main__()

