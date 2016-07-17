#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Tencent Inc.
# All rights reserved.
#
# Author: Chen Feng <phongchen@tencent.com>
#
# Check submits that are not submitted by submit.py

import collections
import datetime
import subprocess
import time
import xml.dom.minidom

import send_mail


_USERNAME = 'GDT_CODE@tencent.com'
_PASSWORD = ''


LogEntry = collections.namedtuple('LogEntry', ['revision', 'author', 'date', 'msg', 'paths'])


def get_logentries(revision):
    logentries = []
    svn_log_xml = subprocess.check_output("svn log -r '%s:HEAD' --xml --verbose" % revision, shell=True)
    doc = xml.dom.minidom.parseString(svn_log_xml)
    log = doc.getElementsByTagName('log')[0]
    for node in log.getElementsByTagName("logentry"):
        revision = node.getAttribute('revision')
        author = node.getElementsByTagName('author')[0].childNodes[0].data
        if node.getElementsByTagName('msg'):
            msg = node.getElementsByTagName('msg')[0].childNodes[0].data
        else:
            msg = ''
        date = node.getElementsByTagName('date')[0].childNodes[0].data
        paths = []
        paths_node = node.getElementsByTagName('paths')[0]
        for path_node in paths_node.getElementsByTagName('path'):
            path = path_node.childNodes[0].data
            paths.append(path)
        now = time.time()
        offset = datetime.datetime.fromtimestamp(now) - datetime.datetime.utcfromtimestamp(now)
        date = str(datetime.datetime.strptime(date, "%Y-%m-%dT%H:%M:%S.%fZ") + offset)
        logentries.append(LogEntry(revision, author, date, msg, paths))

    if logentries:
        logentries = logentries[1:]
    return logentries


# We allow some dir such as java project, does not commit with submit.py.
EXCEPT_PATHS = [
    '/qzone_adsdev_proj/trunk/java/BizRd',
    '/qzone_adsdev_proj/trunk/app/qzap/proto/db',
]


def is_checked_path(path):
    """Whether a path is a except dir that allow not committed by submit.py"""
    for prefix in EXCEPT_PATHS:
        if path.startswith(prefix):
            return False
    return True


def is_committed_by_submit_py(logentry):
    if "Issue: " not in logentry.msg:
        return False
    if "Digest: " not in logentry.msg:
        return False
    if "--crid=" not in logentry.msg:
        return False
    lines = logentry.msg.splitlines()
    if len(lines) < 4:
        return False

    return True


def is_invalid_submit(logentry):
    if is_committed_by_submit_py(logentry):
        return False
    for path in logentry.paths:
        if is_checked_path(path):
            return True
    # expect case
    return False


def get_invalid_submits(revision):
    logentries = get_logentries(revision)
    if not logentries:
        return 0, set(), []
    last_revision = int(logentries[-1].revision)
    authors = set()
    invalid_logentries = []
    for logentry in logentries:
        if is_invalid_submit(logentry):
            authors.add(logentry.author)
            invalid_logentries.append(logentry)
    return last_revision, authors, invalid_logentries


# Peoples who need to audit code changes.
_WATCHERS = [
    'phongchen', 'huanyu', 'jefftang', 'justinyang', 'helenachen', 'emilymou',
    'sidoqin',
]

# Peoples who should also be notified to.
_CC = [
    'g_PerfAds_TL',
]


def mail_notify(authors, logentries):
    html = []
    html.append('<table border="1">')
    html.append('<tr><th>author</th><th>time</th><th>revision</th><th>message</th></tr>')

    for logentry in logentries:
        html.append('<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>' % (
            logentry.author, logentry.date, logentry.revision, logentry.msg))
    html.append('</table>')

    send_mail.send_mail_html(
            user_name=_USERNAME,
            password=_PASSWORD,
            to=list(authors) + _WATCHERS,
            cc=_CC,
            subject='以下未用 app/qzap/common/tools/submit.py 提交',
            text="%s\n" % ';'.join(authors),
            html='\n'.join(html))


def check_submit():
    try:
        revision = int(open('check_submit.history').read())
    except IOError:
        revision = 0
    global _PASSWORD
    _PASSWORD = open('check_submit.password').read().strip()
    last_revision, authors, logentries = get_invalid_submits(revision)
    if last_revision > revision:
        if logentries:
            mail_notify(authors, logentries)
        else:
            print 'Every thing is ok.'
        open('check_submit.history', 'w').write(str(last_revision))
    else:
        print 'No change since last check.'


if __name__ == '__main__':
    check_submit()
