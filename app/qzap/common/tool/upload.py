#!/usr/bin/env python

"""
upload script
Authors:
    Cheng Tao <timcheng@tencent.com>
"""
import ConfigParser
import json
import os
import optparse
import re
import subprocess
import string
import sys
import tempfile

import common
import cppcheck
import cpplint_diff
import secure_scan
import tcr
from common import find_blade_root_dir

def _error_exit(msg):
    common.error_exit('Upload Error: %s' % msg)


class CodeUploader(object):
    _OPTION_GROUP_NAME = "Upload options"
    def __init__(self, argv):
        self.argv = argv
        self.parser = tcr.getParser()
        group = self.parser.add_option_group(CodeUploader._OPTION_GROUP_NAME)
        group.add_option("--no-cpplint", action="store_true", default=False,
                dest="no_cpplint", help="Do not run cpplint before upload.")
        group.add_option("--cppcheck", action="store_true", default=False,
                dest="cppcheck", help="Run cppcheck before upload.")
        group.add_option("--check-missing-tests", action="store_true", default=False,
                dest="check_missing_tests",
                help="Check missing tests for added modules before upload.")
        self.parser.parse_args(argv[1:])
        self.options, self.args = tcr.setInitparser(self.parser).parse_args(argv[1:])
        self.current_source_dir = find_blade_root_dir()
        self.cpplint_err_msgs = []
        self.issue_dict = common.load_issue_info(self.current_source_dir)

    def _svn_up_to_date(self):
        targets = ['BLADE_ROOT', 'thirdparty', 'app/qzap/common/tool', 'app/qzap/tools']
        target_paths = [os.path.join(self.current_source_dir, t) for t in targets]
        if subprocess.call('svn up %s' % ' '.join(target_paths), shell = True) != 0:
            _error_exit('svn up workspace failed.')

    def _check_forbidden_functions(self, cc_files):
        secure_scan.check(cc_files)

    def _check_missing_tests(self, args):
        if not self.options.check_missing_tests:
            return
        diff_files = common.get_added_files(self.args)
        cc_files = common.filter_cc_files(diff_files)
        missings = []
        for f in cc_files:
            if f.endswith('.h'):
                test_file = os.path.splitext(f)[0] + '_test.cc'
                if not os.path.isfile(test_file):
                    missings.append(f)
        if missings:
            common.warning('The following files has no tests:\n'
                    '%s' % '\n'.join(missings))
            answer = raw_input('Continue?(y/N) ').strip()
            if answer != 'y':
                _error_exit('Exit')

    def _cppcheck(self, cc_files):
        if self.options.cppcheck and cc_files:
            if cppcheck.cppcheck(cc_files):
                _error_exit('Cppcheck find errors')

    def _cpplint(self, cc_files):
        if not self.options.no_cpplint and cc_files:
            old_err_msgs, new_err_msgs = cpplint_diff.cpplint_diff(
                                             self.current_source_dir, cc_files)
            if old_err_msgs:
                common.warning("Existed cpplint errors:\n%s" % "\n".join(old_err_msgs))
            if new_err_msgs:
                common.error("Your change cause more new cpplint errors:\n%s"
                             % ("\n".join(new_err_msgs)))
            if old_err_msgs or new_err_msgs:
                tips = ""
                if new_err_msgs:
                    tips = "The red parts will be uploaded to codereview!\n"
                answer = raw_input('%sContinue?(y/N) ' % tips).strip()
                if answer != 'y':
                    _error_exit('Exit')
            self.cpplint_err_msgs = new_err_msgs

    def _check_upload_path_change(self):
        if not self.options.issue:
            return
        issue_id = str(self.options.issue)
        relative_path = os.path.relpath(common.get_cwd(),
                           self.current_source_dir)
        # backward compatible
        if (issue_id in self.issue_dict and
            isinstance(self.issue_dict[issue_id], dict) and
            "upload_path" in self.issue_dict[issue_id]):
              old_upload_path = self.issue_dict[issue_id]["upload_path"]
              if old_upload_path != relative_path:
                  common.warning("the upload path: %s is not the same as "
                          "the last one: %s" % (relative_path, old_upload_path))
                  answer = raw_input('Continue?(y/N) ').strip()
                  if answer != 'y':
                      common.error_exit('Exit')
                  else:
                      self.issue_dict[issue_id]["upload_path"] = relative_path

    def _get_upload_diff_filelist(self, issue_id):
        relative_path = os.path.relpath(common.get_cwd(),
                            self.current_source_dir)
        file_list = []
        if self.options.files:
            file_list = common.get_all_diff_files(self.options.files)
        else:
            file_list = common.get_all_diff_files()
        file_list = set([os.path.join(relative_path, filename) for filename in file_list])
        if issue_id not in self.issue_dict:
            self.issue_dict[issue_id] = {"filelist":file_list,
                                         "upload_path":relative_path}
        else:
            if isinstance(self.issue_dict[issue_id], dict):
                self.issue_dict[issue_id]["filelist"] = file_list
            else:
                self.issue_dict[issue_id] = file_list

    def _get_issue_id(self, upload_result):
        issue_id = ""
        if self.options.issue:
            issue_id = str(self.options.issue)
        else:
            match_pattern = re.compile(r"id:(\d+).*success")
            with open(upload_result) as fd:
                for line in fd:
                    match_object = match_pattern.search(line)
                    if match_object:
                        issue_id = match_object.group(1)
                        break
        if not issue_id:
            _error_exit("Failed to find issue_id! Maybe upload failed!")
        return issue_id

    def _send_cpplint_result_to_comments(self, issue_id):
        # get trunk url of current dir
        cmd = ["svn", "info"]
        svn_info, returncode = common.run_shell(cmd)
        if returncode:
            error_exit("failed to run '%s'" % " ".join(cmd))
        current_trunk_dir = svn_info.split('\n')[2].split()[1]
        comment = []
        for msg in self.cpplint_err_msgs:
            err_info = msg.split(":")
            if len(err_info) < 3:
                continue
            temp_dict = {}
            temp_dict["file"] = os.path.join(current_trunk_dir, err_info[0])
            temp_dict["line"] = err_info[1]
            temp_dict["type"] = "3"
            temp_dict["comment"] = "CPPLINT:" + err_info[2]
            comment.append(temp_dict)
        summary = ('cpplint check result: %d new errors found' %
                   len(self.cpplint_err_msgs))
        json_content = {"comments":comment, "summary":summary}
        (fd, comment_file) = tempfile.mkstemp(suffix = ".json")
        with open(comment_file, 'w') as fd:
            json.dump(json_content, fd)
        # send comment
        cmd = ["%s/app/qzap/common/tool/tcr.py" % self.current_source_dir]
        cmd.extend(["-i", issue_id, "--comment", comment_file])
        output, ret = common.run_shell(cmd)
        if ret:
            common.warning("Failed to send comments! ret=%d" % ret)
        os.remove(comment_file)

    def pre_upload(self):
        diff_files = common.get_diff_files(self.args)
        cc_files = common.filter_cc_files(diff_files)
        self._svn_up_to_date()
        self._check_forbidden_functions(cc_files)
        self._check_missing_tests(self.args)
        self._cpplint(cc_files)
        self._cppcheck(cc_files)
        self._check_upload_path_change()

    def upload(self):
        args = ["%s/app/qzap/common/tool/tcr.py" % self.current_source_dir]
        for i in self.argv[1:]:
            group = self.parser.get_option_group(i)
            if group and group.title == CodeUploader._OPTION_GROUP_NAME:
                continue
            args.append(i)
        print "Runing tcr.py, any problem after this please report to RTX 5000."
        tcr_cmd = subprocess.list2cmdline(args)
        (fd, path) = tempfile.mkstemp("upload")
        cmd = subprocess.list2cmdline(['script', '-q', '-c', tcr_cmd, path])
        subprocess.call(cmd, shell = True)
        issue_id = self._get_issue_id(path)
        os.remove(path)

        self._get_upload_diff_filelist(issue_id)
        common.save_issue_info(self.current_source_dir, self.issue_dict)

        if self.cpplint_err_msgs:
            self._send_cpplint_result_to_comments(issue_id)

def main():
    code_uploader = CodeUploader(sys.argv)
    code_uploader.pre_upload();
    code_uploader.upload()

if __name__ == "__main__":
    main()

