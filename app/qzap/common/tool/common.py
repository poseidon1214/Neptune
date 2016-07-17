# Copyright (c) 2013, Tencent Inc.
# All rights reserved.
#
# Common functions for tools
# Author: Chen Feng <phongchen@tencent.com>


import re
import pickle
import os
import sys
import subprocess

CR_HISTORY_DATABASE="~/.gdt_codereview"

def enum(**enums):
    return type('Enum', (), enums)

def error(msg):
    """Print an error message to stderr."""
    msg = '\033[1;31m' + msg + '\033[0m'
    print >>sys.stderr, msg


def error_exit(msg, code=1):
    """Print an error message to stderr and exit."""
    error(msg)
    sys.exit(code)


def warning(msg):
    msg = '\033[1;33m' + msg + '\033[0m'
    print >>sys.stderr, msg


def run_shell(command, env=os.environ):
    """
    Executes a command and returns the output from stdout and the return code.

    """
    env["LC_ALL"] = "en_US.UTF-8"
    p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            universal_newlines=True, env=env)

    output = p.stdout.read()
    p.wait()
    errout = p.stderr.read()
    if errout:
        print >>sys.stderr, errout
    p.stdout.close()
    p.stderr.close()
    return output, p.returncode


def run_command_in_english(cmdlist):
    env = os.environ
    env["LC_ALL"] = "en_US.UTF-8"
    return subprocess.Popen(cmdlist,
                            stdout=subprocess.PIPE,
                            env=env).communicate()[0]


def get_cwd():
    """get_cwd

    os.getcwd() doesn't work because it will follow symbol link.
    os.environ.get('PWD') doesn't work because it won't reflect os.chdir().
    So in practice we simply use system('pwd') to get current working directory.

    """
    p = subprocess.Popen(['pwd'], stdout=subprocess.PIPE, shell=True)
    return p.communicate()[0].strip()


def find_blade_root_dir(working_dir=None):
    """find_blade_root_dir to find the dir holds the BLADE_ROOT file.

    The blade_root_dir is the directory which is the closest upper level
    directory of the current working directory, and containing a file
    named BLADE_ROOT.

    """
    if working_dir is None:
        working_dir = get_cwd()
    blade_root_dir = working_dir
    if blade_root_dir.endswith('/'):
        blade_root_dir = blade_root_dir[:-1]
    while blade_root_dir and blade_root_dir != '/':
        if os.path.isfile(os.path.join(blade_root_dir, 'BLADE_ROOT')):
            break
        blade_root_dir = os.path.dirname(blade_root_dir)
    if not blade_root_dir or blade_root_dir == '/':
        error_exit(
                "Can't find the file 'BLADE_ROOT' in this or any upper directory.\n"
                "Blade need this file as a placeholder to locate the root source directory "
                "(aka the directory where you #include start from).\n"
                "You should create it manually at the first time.")
    return blade_root_dir


def find_file_bottom_up(filename, from_dir=None):
    """find_blade_root_dir to find the dir holds the BLADE_ROOT file.

    The finding_dir is the directory which is the closest upper level
    directory of the current working directory, and containing a file
    named BLADE_ROOT.

    """
    if from_dir is None:
        from_dir = get_cwd()
    finding_dir = os.path.abspath(from_dir)
    while True:
        path = os.path.join(finding_dir, filename)
        if os.path.isfile(path):
            return path
        if finding_dir == '/':
            break
        finding_dir = os.path.dirname(finding_dir)
    return ''


def _get_svn_diff_files(args, cond):
    diff_files = []
    cmd = ["svn", "st", "-q"]
    if args:
        cmd.extend(args)
    (data, returncode) = run_shell(cmd)
    if returncode:
        error_exit("Diff files not found")
    for line in data.splitlines():
        if cond(line):
            diff_files.append(line[7:].strip())
    return diff_files

def svn_diff_added(filenames):
    """Get svn diff result for each_file, return dict{"filename":[block_one,
    block_two, ...]}, block contains the start_line and
    end_line number(1 bigger) of the diff content which startswith '+', e.g.
    if we get svn diff result for filename like:
        @@ -135,2, +135,3 @@
        AAAA
        -BBBB
        +CCCC
        +DDDD
        we will get block = [136, 138]
    """
    if not filenames:
        return
    cmd = ["svn", "diff"]
    cmd.extend(filenames)
    (data, returncode) = run_shell(cmd)
    if returncode:
        error_exit("Failed to run '%s'" % " ".join(cmd))
    diff_result = data.splitlines()
    result = {}
    file_diff_start_pos = [pos for (pos, value) in enumerate(diff_result)
                           if value.startswith('Index: ')]
    file_diff_len_list = [file_diff_start_pos[i+1] - file_diff_start_pos[i]
                          for i in range(len(file_diff_start_pos)-1)]
    file_diff_len_list.append(
            len(diff_result) - file_diff_start_pos[-1])
    for (file_diff_index, file_diff_begin) in enumerate(file_diff_start_pos):
        file_name = diff_result[file_diff_begin].split()[1] # Index: xxx.cc
        line_index = 0
        diff_content_begin = file_diff_begin + 4
        modify_block_list = []
        i = 0
        file_diff_len = file_diff_len_list[file_diff_index] - 4
        while i < file_diff_len:
            content_line = diff_content_begin + i
            if diff_result[diff_content_begin+i].startswith("@@"):
                # update line_index
                find_line_index = re.search(r'\+(\d+)',
                                      diff_result[content_line])
                if not find_line_index:
                    error_exit("Wrong svn diff format!")
                else:
                    line_index = int(find_line_index.group(1))
                    i += 1
                    continue
            if diff_result[content_line].startswith('-'):
                i += 1
                continue
            if diff_result[content_line].startswith('+'):
                modify_block_len = 1
                while (i + modify_block_len < file_diff_len and
                       diff_result[content_line+modify_block_len].startswith('+')):
                    modify_block_len += 1
                modify_block_list.append((line_index, line_index+modify_block_len))
                i = i + modify_block_len - 1
            i += 1
            line_index += 1
        result[file_name] = modify_block_list
    return result

def get_diff_files(args=None):
    return _get_svn_diff_files(args, lambda line: not line.startswith("D "))


def get_added_files(args=None):
    return _get_svn_diff_files(args, lambda line: line.startswith("A "))


def get_all_diff_files(args=None): # get_diff_files, include deleted file
    return _get_svn_diff_files(args, lambda line: True)


def filter_cc_files(files):
    cc_files = []
    for f in files:
        ext = os.path.splitext(f)[1]
        if not ext in (".h", ".c", ".cpp", ".h", ".hpp", ".C",
            ".cxx", ".cc") or f.endswith("banned_tx.h"):
            continue
        cc_files.append(f)
    return cc_files

def save_issue_info(workspace, obj):
    # use inode to avoid workspace with same name(eg. use mv to change name)
    workspace_inode = os.stat(workspace).st_ino
    issue_info_file = os.path.expanduser(CR_HISTORY_DATABASE) + "_" +\
                          str(workspace_inode)
    with open(issue_info_file, 'wb') as f:
        pickle.dump(obj, f, pickle.HIGHEST_PROTOCOL)

def load_issue_info(workspace):
    workspace_inode = os.stat(workspace).st_ino
    issue_info_file = os.path.expanduser(CR_HISTORY_DATABASE) + "_" +\
                          str(workspace_inode)
    if not os.path.isfile(issue_info_file):
        return {}
    with open(issue_info_file, 'rb') as f:
        return pickle.load(f)

