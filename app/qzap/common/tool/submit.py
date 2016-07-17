#!/usr/bin/env python

"""
Auto commit script
Authors:
    Cheng Tao <timcheng@tencent.com>
"""

from optparse import OptionParser
import hashlib
import json
import optparse
import os
import re
import subprocess
import sys
import tempfile
import urllib
import urllib2

import common
import secure_scan
from common import error_exit
from common import get_cwd
from common import find_blade_root_dir
from common import find_file_bottom_up
from common import run_command_in_english
from common import run_shell
from common import warning


DEFAULT_REVIEW_INFO_URL="http://code.oa.com/tcr-qs/t/tcr/web/endpoint/c/request/getRequestById?requestId="

DEFAULT_REVIEW_URL="http://code.oa.com/v2/user/request/"

_EXT_IGNORES = frozenset([
    '.bak',
    '.diff',
    '.log',
    '.new',
    '.old',
    '.orig',
    '.tmp',
])

_ISSUE_STATE = common.enum(UNDERREVIEW=30, PASSED=45, CLOSED=50, SUBMITED=70)

class SvnInfo(object):
    def __init__(self, path):
        info = run_command_in_english(["svn", "info", path])
        self.svn_info = "\n".join(info.split("\n")[1:])

    def get_remote_path(self):
        for i in self.svn_info.split("\n"):
            if i.startswith("URL: "):
                return i[5:].strip()
        return ""

    def get_revision(self):
        for i in self.svn_info.split("\n"):
            if i.startswith("Revision: "):
                return i.split(":")[1].strip()
        return ""

    def get_last_changed_revision(self):
        for i in self.svn_info.split("\n"):
            if i.startswith("Last Changed Rev: "):
                return i.split(":")[1].strip()
        return ""


def svn_status(path):
    status = run_command_in_english(["svn", "status", path])
    return [tuple(line.split()) for line in status.splitlines()]


class IssueCommitter(object):
    def __init__(self, options):
        self.options = options
        self.issue = options.issue
        self.issue_info_url = "%s%s" % (DEFAULT_REVIEW_INFO_URL, options.issue)
        self.issue_url = "%s%s" % (DEFAULT_REVIEW_URL, options.issue)
        self.user = os.environ.get('USER') or os.environ.get('USERNAME')
        self.issue_info = json.load(urllib.urlopen(self.issue_info_url))

        if not self.issue_info['successfully']:
            print self.issue_info['successfully']
            error_exit("Invalid issue or server down")
        self.issue_info_detail = self.issue_info["requestsWithPagerInfo"]["requests"][0]

        self.local_svn_info = SvnInfo('.')
        self.remote_svn_info = SvnInfo(self.local_svn_info.get_remote_path())
        self.svn_status = svn_status('.')
        self.blade_root_dir = find_blade_root_dir()

    def check_svn_missing(self):
        def maybe_missing(status):
            if not '?' in status:
                return False
            filename = status[1]
            ext = os.path.splitext(filename)[1]
            return ext not in _EXT_IGNORES

        missings = [st[1] for st in filter(maybe_missing, self.svn_status)]
        if missings:
            warning('The following files has not been added to svn:\n'
                    '%s' % '\n'.join(missings))
            answer = raw_input('Continue?(y/N) ').strip()
            if answer != 'y':
                error_exit('Exit')

    def svn_up_to_date(self):
        if subprocess.call('svn up %s' % self.blade_root_dir, shell=True) != 0:
            error_exit('svn up workspace faile.')

    def check_approve(self):
        if not self.issue_info_detail['usersPassRequest']:
            error_exit("Not approved.")

    def expand_depended_targets(self, build_targets):
        if not self.options.build_dependeds:
            return build_targets
        blade = find_file_bottom_up('blade')
        dot_file = 'depended_targets'
        cmd = "%s query --depended --output-to-dot %s %s" % (blade, dot_file,
              build_targets)
        (output, ret) = run_shell(cmd.split())
        all_targets = set()
        if not ret:
            with open(dot_file) as f:
                next(f)  # skip the starting line "digraph blade..."
                for line in f:
                    if line.find('[label =') == -1:
                        break
                    # line: "path:target" [label = "path:target"]
                    all_targets.add(line.split()[0][1:-1])  # skip double quote
            if all_targets:
                build_targets = ''
                for target in sorted(list(all_targets)):
                    build_targets += " %s/%s" % (self.blade_root_dir, target)
            os.remove(dot_file)
        return build_targets

    def cc_build_and_runtests(self):
        action = 'test'
        if self.options.no_test:
            action = 'build'
        blade = find_file_bottom_up('blade')
        if self.options.m:
            mode = "-m%s" % self.options.m
        else:
            mode = ""
        build_targets = self.options.build_targets.replace(",", " ")
        build_targets = self.expand_depended_targets(build_targets)
        p = subprocess.Popen("%s %s %s %s" % (blade, action, mode, build_targets),
                shell=True)
        status = os.waitpid(p.pid, 0)[1]
        if status:
            sys.exit(1)

    def java_build_and_runtests(self, pom_path):
        env = os.environ
        path = env.get('PATH')
        if path:
            env['PATH'] = path + ':' + self.blade_root_dir
        else:
            env['PATH'] = self.blade_root_dir
        p = subprocess.Popen("mvn test", shell=True, env=env,
                             cwd=os.path.dirname(pom_path))
        status = os.waitpid(p.pid, 0)[1]
        if status:
            sys.exit(1)

    def buck_build_and_runtests(self):
        buck = os.path.join(self.blade_root_dir, 'buck')
        p = subprocess.Popen("%s test --all" % buck, shell=True)
        status = os.waitpid(p.pid, 0)[1]
        if status:
            sys.exit(1)

    def build_and_runtests(self):
        pom_path = find_file_bottom_up('pom.xml')
        current_dir = get_cwd()
        java_dir = self.blade_root_dir + '/java/'

        if pom_path:
            self.java_build_and_runtests(pom_path)
        else:
            self.cc_build_and_runtests()
            if java_dir in current_dir:
                self.buck_build_and_runtests()

    def run_presubmit(self):
        if self.options.no_build:
            return
        presubmit = find_file_bottom_up('presubmit')
        if presubmit and os.access(presubmit, os.X_OK):
            presubmit_dir = os.path.dirname(presubmit)
            p = subprocess.Popen(presubmit, shell=True, cwd=presubmit_dir)
            if p.wait() != 0:
                error_exit('presubmit error')
        else:
            self.build_and_runtests()

    def check_upload_submit_consistency(self):
        issue_id = self.issue
        self.issue_dict = common.load_issue_info(self.blade_root_dir)
        if not self.issue_dict.has_key(issue_id):
            warning('issue %s not found in issue_info_dict' % issue_id)
            return
        # TODO(naicaisun): submit.py support --files like upload.py
        relative_path = os.path.relpath(common.get_cwd(),
                            self.blade_root_dir)
        file_list = common.get_all_diff_files()
        file_list = set([os.path.join(relative_path, filename) for filename in file_list])
        # TODO(naicaisun): delete backward compatible after running serveral weeks
        difference_files = []
        if isinstance(self.issue_dict[issue_id], set):
            difference_files = file_list.symmetric_difference(
                    self.issue_dict[issue_id])
        else:
            issue_info = self.issue_dict[issue_id]
            if issue_info["upload_path"] != relative_path:
                error_exit('the submit path: %s is not the same as upload path'
                           ' : %s' % (relative_path,
                           issue_info["upload_path"]))
            difference_files = file_list.symmetric_difference(issue_info["filelist"])
        if difference_files:
            warning('the following file[s] you submit are not consistent with '
                    'the ones you uploaded\n%s' % "\n".join(difference_files))
            answer = raw_input('Continue?(y/N) ').strip()
            if answer != 'y':
                error_exit('Exit')
            else:
                if isinstance(self.issue_dict[issue_id], set):
                    self.issue_dict[issue_id] = file_list
                else:
                    self.issue_dict[issue_id]["filelist"] = file_list

    """find path contains BUILD whose content contains filename
    """
    def _find_build_path_contain_filename_bottom_up(self, filename):
        abs_dir = os.path.normpath(
                      os.path.join(self.blade_root_dir, filename))
        finding_dir = os.path.dirname(abs_dir)
        file_in_src = os.path.basename(abs_dir)
        path_sep_pos = len(finding_dir)
        while True:
            path = os.path.join(finding_dir, 'BUILD')
            if os.path.isfile(path):
                with open(path) as f:
                    for line in f:
                        if line.find(file_in_src) != -1:
                            return finding_dir
            if path_sep_pos < len(self.blade_root_dir):
                return ""
            path_sep_pos = abs_dir.rfind(os.path.sep, 0, path_sep_pos)
            if path_sep_pos == -1:
                return ""
            finding_dir = abs_dir[:path_sep_pos]
            file_in_src = abs_dir[(path_sep_pos + len(os.path.sep)):]

    """check whether issue_to_submit depends on issue
    """
    def _has_dependency_relation(self, submit_issue_deps, issue):
        file_list = []
        if isinstance(self.issue_dict[issue], set):
            file_list = self.issue_dict[issue]
        else:
            file_list = self.issue_dict[issue]["filelist"]
        found_build_path = set()
        for f in file_list:
            find_path = self._find_build_path_contain_filename_bottom_up(f)
            if find_path and find_path not in found_build_path:
                found_build_path.add(find_path)
                build_file = os.path.join(find_path, 'BUILD')
                with open(build_file) as f:
                    path_in_trunk = os.path.relpath(find_path,
                                                    self.blade_root_dir)
                    for line in f:
                        match_obj = re.match(r"\s*name\s*=\s*'(\w+)'.*", line)
                        if match_obj:
                            target = path_in_trunk + ":" + match_obj.groups()[0]
                            if target in submit_issue_deps:
                                return True
        return False

    """if multiple issues not submit in current workspace(eg. A, B), if A
    depends on B, A should not be submited before B
    """
    def check_dependency_between_issues(self):
        if not self.issue_dict.has_key(self.issue) or len(self.issue_dict) < 2:
            return
        build_path = set()
        current_dir = get_cwd()
        file_list = []
        if isinstance(self.issue_dict[self.issue], set):
            file_list = self.issue_dict[self.issue]
        else:
            file_list = self.issue_dict[self.issue]["filelist"]
        for f in file_list:
            ext = os.path.splitext(f)[1]
            #BUILD has no .h dependency specification, so not check .h
            if ext in (".c", ".cpp", ".hpp", ".C", ".cxx", ".cc"):
                find_path = self._find_build_path_contain_filename_bottom_up(f)
                if find_path:
                    build_path.add(os.path.relpath(find_path, current_dir))
        if not build_path:
            # some issues have no associated BUILD file(eg. modify script files)
            return
        blade = find_file_bottom_up('blade')
        cmd = "%s query --deps %s" % (blade, " ".join(build_path))
        (output, ret) = run_shell(cmd.split())
        if ret:
            warning('failed to run %s' % cmd)
            return
        submit_issue_deps = set(output.splitlines())
        issues_to_pop = []
        for issue in self.issue_dict.keys():
            if issue == self.issue:
                continue
            issue_info_url = "%s%s" % (DEFAULT_REVIEW_INFO_URL, issue)
            issue_info = json.load(urllib.urlopen(issue_info_url))
            if not issue_info['successfully']:
                warning('failed to get issue_info for issue %s' % issue)
                continue
            issue_info_detail = issue_info["requestsWithPagerInfo"]["requests"][0]
            issue_state = issue_info_detail['state']
            if (issue_state == _ISSUE_STATE.CLOSED or
                issue_state == _ISSUE_STATE.SUBMITED):
                # issue already closed or submited
                issues_to_pop.append(issue)
                continue
            if self._has_dependency_relation(submit_issue_deps, issue):
                warning('the submit issue may depends on the issue %s with'
                        ' title \"%s\"' % (issue, issue_info_detail['name']))
                answer = raw_input('Continue?(y/N) ').strip()
                if answer != 'y':
                    error_exit('Exit')
        if issues_to_pop:
            map(self.issue_dict.pop, issues_to_pop)

    def check(self):
        self.check_svn_missing()
        self.svn_up_to_date()
        self.run_presubmit()
        diff_files = common.get_diff_files()
        secure_scan.check(diff_files)
        self.check_upload_submit_consistency()
        self.check_dependency_between_issues()

        # Put this check at last to allow dry-run before approve
        self.check_approve()

    def commit_to_svn(self):
        title = self.issue_info_detail["name"]
        (fd, path) = tempfile.mkstemp("svn_commit")
        f = open(path, "w")
        print >>f, title.encode('utf-8')
        print >>f, "Issue: %s" % self.issue_url
        print >>f, "--crid=%d" % self.issue_info_detail["id"]
        m = hashlib.md5()
        m.update(title.encode('UTF-8'))
        m.update(self.issue_url)
        print >>f, "Digest: %s" % m.hexdigest()
        f.close()
        os.close(fd)
        filenames = '.'
        cmd = "svn commit -F %s ." % path
        p = subprocess.Popen(cmd, shell=True)
        p.wait()
        os.remove(path)
        if p.returncode != 0:
            error_exit("Not committed")
        else:
            if self.issue_dict.has_key(self.issue):
                self.issue_dict.pop(self.issue)
                common.save_issue_info(self.blade_root_dir, self.issue_dict)


def main():
    parser = OptionParser()
    parser.add_option("-i", "--issue", dest="issue",
                      help="Codereview issue number.")
    parser.add_option("--no-test", dest="no_test", action="store_true",
                      help="Don't test before commit, just build.")
    parser.add_option("-n", "--no-build", dest="no_build", action="store_true",
                      help="Don't build and test before commit.")
    parser.add_option("-m", dest="m",
                      help="Target bits")
    parser.add_option("--build-targets", dest="build_targets", default="...",
                      help="Targets to build, blade target format, "
                      "separated by ','")
    parser.add_option("--build-dependeds", dest="build_dependeds", action="store_true",
                      help="Also build depended targets to make more safe")
    parser.add_option("--dry-run", dest="dry_run", action="store_true",
                      help="Just run presubmit checks, not commit.")

    (options, args) = parser.parse_args()
    if not options.issue:
        parser.print_help()
        error_exit("")

    committer = IssueCommitter(options)
    committer.check()
    if options.dry_run:
        print "All presubmit checks success"
    else:
        committer.commit_to_svn()


if __name__ == "__main__":
    main()
