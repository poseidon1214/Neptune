#!/usr/bin/env python
# Copyright (c) 2014 Tencent Inc.
# Author: XiaoZhipeng <macxiao@tencent.com>

"Pkg submitter interface"

from optparse import OptionParser
import json
import sys
import os
import urllib2
import time
import locale
import subprocess
import logging

class PkgSubmitter(object):
    def __init__(self, ip, product_name, pkg_name):
        self.ip = ip
        self.url = 'http://10.166.224.35/interface_v2/pkginterface.php'
        self.product_name = product_name
        self.pkg_name = pkg_name
        logging.basicConfig(filename = os.path.join(os.getcwd(), 'pkg_log.txt'),
            level = logging.DEBUG,
            format = '%(asctime)s - %(levelname)s: %(message)s')
        self.log = logging.getLogger('root')

    def make_request(self, interfaceName, para):
        data = {'version':1.0, 'caller':'gdt_mixer', 'password':'gdt_mixer',
            'callee':'gdt_mixer','eventId':101, 'timestamp': time.time(),
            'interface':{'interfaceName': interfaceName, 'para':para}}

        return json.dumps(data, sort_keys=True, indent=4, ensure_ascii=False)

    def send_http_request(self, interfaceName, para):
        data = self.make_request(interfaceName, para)

        print "request:\n%s" % data
        self.log.info('request:\n%s', data)

        repeated_times = 10;
        for x in range(repeated_times + 1):
            if x == repeated_times:
                print "Failed to call %s, retcode: %s, msg: %s" % (interfaceName,
                        response_struct["returnValue"],
                        response_struct["returnMsg"])
                sys.exit(-1);
            req = urllib2.Request(self.url, data)
            req.add_header('Host', 'pkg.isd.com')
            response = urllib2.urlopen(req)
            response_data = response.read()
            response_struct = json.loads(response_data)
            if response_struct["returnValue"] == 0:
                break

        print "response: \n%s" % response_data
        self.log.info('response:\n%s', response_data)

        null=''
        return eval(response_data)

    def upload_file(self, file_list, dest_path):
        para = {'ip':self.ip, 'fileList':file_list, 'dest':dest_path}
        self.send_http_request("remoteDownloadFile", para)

    def submit(self, author, description_file, init_file):
        description_f = open(description_file, 'rb')
        init_f = open(init_file, 'rb')
        version = self.generate_new_version()
        pkg_path  = '/%s/%s' % (self.product_name, self.pkg_name)

        para = {'path':pkg_path, 'confProduct':self.product_name,
            'confName':self.pkg_name, 'confVersion':version,
            'confRemark':description_f.read(), 'confUser':'user_00',
            'confAuthor':author, 'confContent':init_f.read(),
            'confFrameworkType':'plugin', 'isShell':'false'}
        self.send_http_request("submitUpdatePackage", para)

        description_f.close()
        init_f.close()

    def get_current_version(self):
        para = {'product':self.product_name, 'name':self.pkg_name}
        response = self.send_http_request("ListPackageVersion", para)

        return response['returnData']['version'][-1]

    def generate_new_version(self):
        current = self.get_current_version()

        # Input : 1.0.369
        # Output: 1.0.370
        new_tag = int(current.split('.')[2]) + 1
        new_version = '%s.%s.%s' % (current.split('.')[0],
            current.split('.')[1], new_tag)
        return new_version

