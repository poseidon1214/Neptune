#!/usr/bin/env python

import common
import re
import subprocess
import sys

"""
#TODO(naicaisun): add whitelist
_EXCEPTION_DIRS = [
    'thirdparty'
]
"""

def cpplint_diff(blade_root_dir, cc_files):
    """partition cpplint result into two parts: the part not imported by your
    change and the yes part
    """
    diff_details =  common.svn_diff_added(cc_files)
    p = subprocess.Popen("%s/app/qzap/tools/cpplint.py %s" % (blade_root_dir,
        " ".join(cc_files)), stderr=subprocess.PIPE, shell=True)
    stdout, stderr = p.communicate()
    if p.returncode and p.returncode != 1:
        # returncode is 0 when no error found, 1 when error found
        common.error_exit('Failed to run cpplint, returncode=%d' % p.returncode)
        return [], []
    match_pattern = re.compile(r":\d+:|^Done processing")
    all_err_msgs = filter(lambda err_msg: match_pattern.search(err_msg),
                          stderr.splitlines())
    err_stat_info = [(line_num, err_msg.split()[2])
                     for line_num, err_msg in enumerate(all_err_msgs) if
                     err_msg.startswith('Done processing')]
    i = 0
    old_err_msgs = []
    new_err_msgs = []
    for err_end_line, filename in err_stat_info:
        if not filename in diff_details:
            # ignore file marked as A+ in svn which has no diff_detail
            i = err_end_line + 1
            continue
        current_diff_info = diff_details[filename]
        j = 0
        while i < err_end_line:
            err_msg = all_err_msgs[i]
            line_num = int(err_msg.split(':')[1])
            while j < len(current_diff_info):
                if line_num < current_diff_info[j][0]:
                    old_err_msgs.append(err_msg)
                    break
                else:
                    if (line_num < current_diff_info[j][1]):
                        new_err_msgs.append(err_msg)
                        break
                    else:
                        j += 1
            if j >= len(current_diff_info):
                old_err_msgs.extend(all_err_msgs[i:err_end_line])
                break
            i += 1
        i = err_end_line + 1
    return old_err_msgs, new_err_msgs

def __main__():
    diff_files = common.get_diff_files(sys.argv[1:])
    cc_files = common.filter_cc_files(diff_files)
    if cc_files:
        blade_root_dir = common.find_blade_root_dir()
        old_err_msgs, new_err_msgs =  cpplint_diff(blade_root_dir, cc_files)
        if old_err_msgs:
            common.warning("%s" % "\n".join(old_err_msgs))
        if new_err_msgs:
            common.error("%s" % ("\n".join(new_err_msgs)))
            sys.exit(1)
    sys.exit(0)

if __name__ == '__main__':
    __main__()
