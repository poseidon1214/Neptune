#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright @2015 Tencent Inc.
# Lifeng Wang (fandywang@tencent.com)

import os
import sys
import random
import optparse
import codecs

def shuffle(input_file, output_file, ienc='utf8'):
    data = []
    fin = codecs.open(input_file, 'r', ienc)
    for line in fin:
        data.append(line)
    fin.close()

    eval_lines = random.sample(range(len(data)), int(0.2 * len(data)))
    
    eval_out = codecs.open(output_file + '.test', 'w', ienc)
    train_out = codecs.open(output_file + '.train', 'w', ienc)
    for i in xrange(0, len(data)):
        if i in eval_lines:
            eval_out.write(data[i])
        else:
            train_out.write(data[i])
    eval_out.close()
    train_out.close()


#----------------------------------------------------------------------------#
# MODULE EPILOGUE
#----------------------------------------------------------------------------#

def create_option_parser():
    """ Creates an option parser instance to handle command-line options.
    """
    usage = \
"""%prog [options]
    shuffle train and eval data.
"""
    parser = optparse.OptionParser(usage)
    parser.add_option('-i', '--input', action='store', dest='input',
            help='the input filename')
    parser.add_option('-o', '--output', action='store', dest='output',
            help='the output filename')

    return parser

#----------------------------------------------------------------------------#

def main(argv):
    """ The main method for this module.
    """
    parser = create_option_parser()
    (options, args) = parser.parse_args(argv)
    if None in [options.input, options.output]:
        parser.print_help()
        print  "\nERROR: please specify -i INPUT -o OUTPUT\n"
        return

    shuffle(options.input, options.output)
    return

#----------------------------------------------------------------------------#

if __name__ == '__main__':
    main(sys.argv[1:])

#----------------------------------------------------------------------------#
