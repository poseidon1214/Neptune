# Copyright (c) 2015 Tencent Inc.
# All rights reserved.
#
# Author: Sun Naicai<naicaisun@tencent.com>
# Date:   December 10, 2015
"""
This is a recordio python interface
"""

import os
from ctypes import *

_LIB_RECORDIO = 'libdynamic_recordio_jna.so'
_module_dir = os.path.dirname(os.path.realpath(__file__))
_lib_dir = _module_dir + os.path.sep + _LIB_RECORDIO
try:
    _librecordio = cdll.LoadLibrary(_lib_dir)
except:
    print 'Load libray', _lib_dir, 'failed!'
    raise
_librecordio.ReadRecord.restype = c_bool
_librecordio.WriteRecord.restype = c_bool
_librecordio.FlushRecordWriter.restype = c_bool

class RecordReader:
    """options:
        RESUME_LAST_INCOMPLETE_BLOCK = 0x0002,
        If we want to read from a record i/o file when a writer is writing
        simultaneously, we might read an incomplete block at the end of
        stream. If this option is set, we still return false for readxxx
        function when getting an incomplete block at the end of stream, but
        would resume from it when user calls readxxx function again.

        NOTE: please use caution when setting this option, it introduces
        ambiguity as we can NOT distinguish a "corrupt" block from an
        "incomplete" block at the end of stream, so only set it when
        necessary, i.e. a writer is writing data simultaneously.
    """
    def __init__(self, filename, options = 0, start = -1, end = -1):
        if start == -1 and end == -1:
            self.record_reader = _librecordio.OpenRecordReader(
                    filename, c_uint(options))
        else:
            self.record_reader = _librecordio.OpenSplitRecordReader(
                    filename, c_uint(options), c_longlong(start),
                    c_longlong(end))
        if not self.record_reader:
            raise IOError, 'failed to create RecordReader'

    def __del__(self):
        if self.record_reader:
            _librecordio.CloseRecordReader(self.record_reader)

    """ReadRecord return reade result(True for success, False for failure),
       record(type: str)
    """
    def read_record(self):
        buf_p = POINTER(c_char)()
        record_size = c_int()
        ret = _librecordio.ReadRecord(
                self.record_reader,
                byref(buf_p),
                byref(record_size))
        if ret:
            return ret, buf_p[:record_size.value]
        else:
            return ret, ""

class RecordWriter:
    """python interface for RecordWriter
    """
    def __init__(self, filename, options = 0, compression_codec = 0):
        self.record_writer = _librecordio.OpenRecordWriter(
                filename,
                c_uint(options), c_uint(compression_codec))
        if not self.record_writer:
            raise IOError, 'Failed to create RecordWriter'

    """flush data and close file
    """
    def __del__(self):
        if self.record_writer:
            _librecordio.CloseRecordWriter(self.record_writer)

    """WriteRecord writes data (type: str) with length 'size' into filename
    """
    def write_record(self, data, size = -1):
        if size == -1:
            size = len(data)
        ret = _librecordio.WriteRecord(
                self.record_writer,
                cast(data, c_char_p),
                c_int(size))
        return ret

    def flush(self):
        return _librecordio.FlushRecordWriter(self.record_writer)
