#!/usr/bin/env python
#coding:utf8

# Copyright (c) 2015, Tencent.
# All Rights Reserved.
# Author: Xinyan Lu (xinyanlu@tencent.com)

# Preprocess the dataset into the cPickle format of Keras, i.e.,
# dataset = [train_X, train_y, test_X, test_y, vocab] with
# feature in train_X and test_X: a sequence of word indexes (integers)
# label in train_y and test_y: int
# vocab: list of words, sorted by occurrences in descend order 

import codecs
import cPickle
import re

def load_taxonomy(filename):
    label_mapping = dict()
    fid = codecs.open(filename, 'r', 'utf-8')
    for line in fid.readlines():
        parts = line.split()
        # retain those Level 1 categories
        if int(parts[3]) == 1:
            label_mapping[parts[1]] = int(parts[0])
    fid.close()
    return label_mapping

def split_label_content(input_corpus_filename, label_mapping, 
                        output_label_filename = None, 
                        output_content_filename = None):
    labels = []
    qq_group_pattern = re.compile('#\s\d+')

    fid = codecs.open(input_corpus_filename, 'r', 'utf-8')
    if output_label_filename == None:
        output_label_filename = input_corpus_filename + './label'
    if output_content_filename == None:
        output_content_filename = input_corpus_filename + './content'

    fout_label = codecs.open(input_corpus_filename + '.label', 'w', 'utf-8')
    fout_content = codecs.open(input_corpus_filename + '.content', 'w', 'utf-8')
    
    num_error = 0
    for line in fid.readlines():
        parts = line.split('\t')
        # Spell mistake correction
        if parts[0] == u'服饰鞋包箱包':
            parts[0] = u'服饰鞋帽箱包'
        
        # get Level 1 category
        idx = parts[0].find('--')
        if idx > 0:
            parts[0] = parts[0][:idx]
        
        # ignore these examples
        if parts[0] == '#N/A':
            num_error += 1
            continue
        elif len(parts) != 2:
            num_error += 1
            continue
        elif len(parts[0]) > 20:
            num_error += 1
            continue

        label = label_mapping.get(parts[0], -1)
        if label == -1:
            print parts[0], '>>'
            print parts[1]
            num_error += 1
        else:
            labels.append(label)
            # Remove QQ group number
            parts[1] = re.sub(qq_group_pattern, '', parts[1].strip())
            
            # retain those examples with at least 4 characters
            if len(parts[1]) >= 4:
                fout_label.write(unicode(label) + '\n')
                fout_content.write(parts[1] + '\n')
    print 'Number of errors:', num_error
    
    fid.close()
    fout_label.close()
    fout_content.close()

def get_corpus_dataset(train_label_filename, train_content_filename,
                       test_label_filename, test_content_filename,
                       output_filename, vocab_size):
    from keras.preprocessing.text import Tokenizer

    # Note the labels and content are seperated. 
    # One line per example.
    train_corpus_label = codecs.open(train_label_filename, 'r', 'utf-8').readlines()
    train_corpus_content = codecs.open(train_content_filename, 'r' ,'utf-8').readlines()

    test_corpus_label = codecs.open(test_label_filename, 'r', 'utf-8').readlines()
    test_corpus_content = codecs.open(test_content_filename, 'r' ,'utf-8').readlines()

    # We need to encode the content in 'str' (instead of 'unicode'), 
    # or raise error in string.translate function
    train_corpus_content_str = [x.encode('utf-8') for x in train_corpus_content]
    test_corpus_content_str = [x.encode('utf-8') for x in test_corpus_content]

    # retaining the top *nb_words* words with highest occurrences
    tokenizer = Tokenizer(nb_words = vocab_size)

    tokenizer.fit_on_texts(train_corpus_content_str)

    # Get sequence index of documents.
    train_corpus_seq = tokenizer.texts_to_sequences(train_corpus_content_str)  
    test_corpus_seq = tokenizer.texts_to_sequences(test_corpus_content_str)

    # Retain the examples with vocubulary words.
    train_X = []
    train_y = []
    for i in range(len(train_corpus_seq)):
        if len(train_corpus_seq[i]) > 0:
            train_X.append(train_corpus_seq[i])
            train_y.append(int(train_corpus_label[i]))

    print 'Number of Train Examples:', len(train_X)

    test_X = []
    test_y = []
    for i in range(len(test_corpus_seq)):
        if len(test_corpus_seq[i]) > 0:
            test_X.append(test_corpus_seq[i])
            test_y.append(int(test_corpus_label[i]))

    print 'Number of Test Examples:', len(test_X)

    # We would like to map the category ids:
    # 1, 2, ..., 15, 16, 22
    # into as follows:
    # 0, 1, 2, ..., 16, 17
    # with id_mapping
    id_mapping = {21:17, 22:0}

    for i in range(len(train_y)):
        if id_mapping.get(train_y[i], -1) != -1:
            train_y[i] = id_mapping[train_y[i]]
    for i in range(len(test_y)):
        if id_mapping.get(test_y[i], -1) != -1:
            test_y[i] = id_mapping[test_y[i]]

    vocab = tokenizer.word_index.keys()
    vocab = sorted(vocab, key = lambda x:tokenizer.word_index[x])

    # save the dataset into 'pkl' file
    fid = open(output_filename,'wb')
    cPickle.dump([train_X, train_y, test_X, test_y, vocab], fid)
    fid.close()

if __name__ == '__main__':
    label_mapping = load_taxonomy('./taxonomy')
    split_label_content(input_corpus_filename = './train.all.new', 
                        label_mapping = label_mapping,
                        output_label_filename = './train.all.new.label',
                        output_content_filename = './train.all.new.content')
    split_label_content(input_corpus_filename = './test.all.new', 
                        label_mapping = label_mapping,
                        output_label_filename = './test.all.new.label',
                        output_content_filename = './test.all.new.content')

    # Then you should have documents segmented with TextMiner:
    # ./train.all.new.content (unsegmented) -> ./train.all.new.content.out (segmented)
    # ./test.all.new.content (unsegmented) -> ./test.all.new.content.out (segmented)
    # for example:
    # ./text_segmenter_main --input_file='./train.all.new.content' 
    #                       --segmenter_data_dir='./tc_data/' 
    #                       --stopword_file='./tc_data/kedict/dict.stopword' 
    #                       --output_file='./train.all.new.content.out'
    
    # run the following line with Keras
    get_corpus_dataset(train_label_filename = './train.all.new.label',
                       train_content_filename = './train.all.new.content.out',
                       test_label_filename = './test.all.new.label',
                       test_content_filename = './test.all.new.content.out',
                       output_filename = './corpus_dataset_with_vocab.pkl', 
                       vocab_size = 50000)
