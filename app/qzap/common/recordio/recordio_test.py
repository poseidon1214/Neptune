# Copyright (c) 2015 Tencent Inc.
# All rights reserved.
#
# Author: Sun Naicai<naicaisun@tencent.com>
# Date:   December 10, 2015

import os
import unittest

from recordio import RecordReader, RecordWriter


class RecordIoTest(unittest.TestCase):
    def setUp(self):
        self.test_file = 'test.txt'
        if os.path.exists(self.test_file):
            os.remove(self.test_file)
        self.record_one = "hello, world"
        # a serialized protobuf message Record
        self.record_two = '\x08\x05\x11\x00\x00\x00\x00\x00\x00\x0c@\x1a\x05hello'
        record_writer = RecordWriter(self.test_file)
        self.assertTrue(record_writer.write_record(self.record_one))
        self.assertTrue(record_writer.write_record(self.record_two))
        del record_writer

    def testReader(self):
        self.assertRaises(IOError, RecordReader,"")
        self.assertTrue(os.path.exists(self.test_file))
        record_reader = RecordReader(self.test_file)

        ret, data = record_reader.read_record()
        self.assertTrue(ret)
        self.assertEqual(data, self.record_one)

        ret, data = record_reader.read_record()
        self.assertTrue(ret)
        self.assertEqual(data, self.record_two)

        ret, data = record_reader.read_record()
        self.assertFalse(ret)

def suite():
    test_suite = unittest.TestSuite()
    test_suite.addTest(RecordIoTest("testReader"))
    return test_suite

if __name__ == "__main__":
    unittest.main(defaultTest = 'suite')
