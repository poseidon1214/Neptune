#!/usr/bin/env python
# coding: utf-8

# Copyright (c) 2015, Tencent.
# All Rights Reserved.
# Author: Xinyan Lu (xinyanlu@tencent.com)

# Load the input-hidden weight matrix from the original C word2vec-tool format,
# and get the vectors of required words.

from __future__ import print_function
import cPickle
import numpy as np

# The file to store the vocabulary vectors, which is a numpy ndarray 
# with shape (1 + max_feature, vector_size).
output_filename = './corpus_vocab_vec.pkl'

# Word2vec vector file (in binary format)
word2vec_filename = '/data/lhotse/jacksonzhao/word2vec-master/train_data/vector_weixin_90gb.bin'

# Corpus dataset = [train_X, train_y, test_X, test_y, vocab] with:
# feature in train_X and test_X: a sequence of word indexes (integers)
# label in train_y and test_y: int
# vocab: list of words, sorted by occurrences in descend order 
corpus_data = cPickle.load(open('./corpus_dataset_with_vocab.pkl','rb'))

# Vocabulary, retaining the top words with highest occurrences 
max_feature = max([max(i) for i in corpus_data[0]])
vocabulary = corpus_data[4][:max_feature]

vocabulary_id = dict()
# idx starting at 1 
idx = 1
for word in vocabulary:
    vocabulary_id[word] = idx
    idx += 1

print('loading projection weights from %s' % word2vec_filename)
fid = open(word2vec_filename, 'rb')

# Load word2vec vocabulary size and vector size
word2vec_vocab_size, vector_size = map(int, fid.readline().split())
print('word2vec vocab_size:', word2vec_vocab_size)
print('word2vec vector_size:', vector_size)

# Note that result[0] is reserved for zero-padding
corpus_vocab_vec = np.zeros((len(vocabulary) + 1, vector_size), dtype=np.float32)

# Each element of vector is stored as an 32-bit float, thus 4 bytes
binary_len = 4 * vector_size
# Number of loaded words
word_loaded_cnt = 0
# Number of decode error
decode_error_cnt = 0
for line_no in xrange(word2vec_vocab_size):
    # Mixed text and binary: read text first, then binary
    if line_no % 200000 == 0:
        print(line_no)
    word = []
    while True:
        ch = fid.read(1)
        if ch == b' ':
            break
        if ch != b'\n':  # ignore newlines in front of words
            word.append(ch)
    try:
        word = b''.join(word).decode('utf-8')
    except Exception, e:
        decode_error_cnt += 1
        continue
    weights = np.fromstring(fid.read(binary_len), dtype=np.float32)

    idx = vocabulary_id.get(word, -1)
    if idx != -1:
        corpus_vocab_vec[idx,:] = weights
        word_loaded_cnt += 1

print('Finish:')
print(decode_error_cnt, 'decode errors encountered.')
print(word_loaded_cnt, 'words loaded.')

# Replace those vector having NaN (Not a Number) with zeros
# Note: If an element of a vector is NaN, 
#       other elements of the vector are unusable.
nan_idx = np.where(np.isnan(np.sum(corpus_vocab_vec, axis=1)))[0]
print('replacing', len(nan_idx), 'words (NaN) with zeros')
corpus_vocab_vec[nan_idx] = np.zeros(vector_size)

# Initialize unknown word vectors, replace zeros with Gaussian(0, 0.1)
# Warning: use at your risk (unknown if this really helps)
# zero_idx = np.where(np.sum(np.abs(corpus_vocab_vec), axis=1) == 0)[0]
# print('initializing', len(zero_idx), 'words (all zeros) with Gaussian(0, 0.1)')
# corpus_vocab_vec[zero_idx] = 0.1 * np.random.randn(len(zero_idx), vector_size)

fout = open(output_filename, 'wb')
cPickle.dump(corpus_vocab_vec, fout)
fout.close()
