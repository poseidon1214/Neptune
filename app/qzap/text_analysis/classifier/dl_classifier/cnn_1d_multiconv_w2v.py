#!/usr/bin/env python
#coding:utf8

# Copyright (c) 2015, Tencent.
# All Rights Reserved.
# Author: Xinyan Lu (xinyanlu@tencent.com)

# Another baseline convolutional neural network
# Reference: 1. A Sensitivity Analysis of (and Practitioner' Guide to) 
#               Convolutional Neural Networks for Sentence Classification
#            2. Fast and Accurate Deep Network Learning by Exponential Linear Units (ELUs)
# Network structure:
# sequence input(length=150) -> embedding (dim=100, initialized with word2vec vectors)
#                            -> | conv1D (num_filters=250, length=2) -> maxpooling (pool=2) |
#                               | conv1D (num_filters=250, length=3) -> maxpooling (pool=2) |
#                            -> full-connected (hidden_unit=250, activation=ELU) 
#                            -> full-connected (hidden_unit=250, activation=tanh)
#                            -> softmax (18)

# Output:
# Loading data...
# 18 classes
# 208034 train sequences
# 12853 test sequences
# Pad sequences (samples x time)
# X_train shape: (208034, 150)
# X_test shape: (12853, 150)
# Convert class vector to binary class matrix (for use with categorical_crossentropy)
# Build model...
# Train on 208034 samples, validate on 12853 samples
# Epoch 1/5
# 208034/208034 [==============================] - 110s - loss: 0.5691 - val_loss: 0.5049
# Epoch 2/5
# 208034/208034 [==============================] - 110s - loss: 0.2492 - val_loss: 0.4322
# Epoch 3/5
# 208034/208034 [==============================] - 110s - loss: 0.1902 - val_loss: 0.4361
# Epoch 4/5
# 208034/208034 [==============================] - 110s - loss: 0.1576 - val_loss: 0.4439
# Epoch 5/5
# 208034/208034 [==============================] - 110s - loss: 0.1364 - val_loss: 0.4571
# Accuracy         : 0.8828
# F-score   (Micro): 0.8650
# Precision (Micro): 0.8712
# Recall    (Micro): 0.8588
# F-score   (Macro): 0.8273
# Precision (Macro): 0.8445
# Recall    (Macro): 0.8130
#   Id     Examples    Predicted      Match    Precision     Recall     F-Score 
# ===============================================================================
#    0         5615         5718       5130       0.8972     0.9136      0.9053 
#    1         1068         1135        973       0.8573     0.9110      0.8833 
#    2          241          216        190       0.8796     0.7884      0.8315 
#    3          450          443        421       0.9503     0.9356      0.9429 
#    4          322          318        296       0.9308     0.9193      0.9250 
#    5          278          294        256       0.8707     0.9209      0.8951 
#    6          197          185        156       0.8432     0.7919      0.8168 
#    7           60           56         43       0.7679     0.7167      0.7414 
#    8          172          174        137       0.7874     0.7965      0.7919 
#    9         1306         1272       1148       0.9025     0.8790      0.8906 
#   10         1096         1088        925       0.8502     0.8440      0.8471 
#   11           52           52         41       0.7885     0.7885      0.7885 
#   12          396          407        327       0.8034     0.8258      0.8144 
#   13          199          172        161       0.9360     0.8090      0.8679 
#   14          152          119         93       0.7815     0.6118      0.6863 
#   15           91           79         63       0.7975     0.6923      0.7412 
#   16         1080         1046        929       0.8881     0.8602      0.8739 
#   17           78           79         57       0.7215     0.7308      0.7261 
# ===============================================================================

from __future__ import print_function
import cPickle

import numpy as np
np.random.seed(1337)  # for reproducibility. However it is not guaranteed with current cuDNN implementation. 

from keras.callbacks import EarlyStopping
from keras.preprocessing import sequence
from keras.models import Graph
from keras.layers import containers
from keras.layers.core import Dense, Dropout, Activation, Flatten, Reshape
from keras.layers.embeddings import Embedding
from keras.layers.convolutional import Convolution1D, MaxPooling1D
from keras.layers.advanced_activations import ELU
from keras.utils import np_utils

import classifier_eval

# Retain the first *maxlen* words in a document
maxlen = 150
# Retain the top *max_features* words with highest occurrences
max_features = 50000
# Training batch size
batch_size = 256
# Embedding size of each word
embedding_dims = 100
# Number of convolutional filters
nb_filter = 250
# Length of convolutional filters
filter_lengths = [2,3]
# Number of hidden units in the full-connected layer
hidden_dims = 250
# Training epoches
nb_epoch = 5
# Length of max pooling 
pool_length = 2

# Note: the neural network has many parameters, some params (e.g., activation 
#       function) can be set in the model definition part of code (see below).

print('Loading data...')

# Labeled corpus data from './corpus_preprocess.py'
# data = [X_train, y_train, X_test, y_test, vocab]
# feature: a sequence of word indexes
# label: int (category id, starting at 0)
data = cPickle.load(open('./corpus_dataset_with_vocab.pkl', 'rb'))

X_train = data[0]
y_train = data[1]
X_test = data[2]
y_test = data[3]

w2v_word_vec = cPickle.load(open('./corpus_vocab_vec.pkl', 'rb'))

nb_classes = max(y_train) + 1
print(nb_classes, 'classes')

print(len(X_train), 'train sequences')
print(len(X_test), 'test sequences')

print('Pad sequences (samples x time)')
X_train = sequence.pad_sequences(X_train, maxlen=maxlen)
X_test = sequence.pad_sequences(X_test, maxlen=maxlen)
print('X_train shape:', X_train.shape)
print('X_test shape:', X_test.shape)

print('Convert class vector to binary class matrix (for use with categorical_crossentropy)')
Y_train = np_utils.to_categorical(y_train, nb_classes)
Y_test = np_utils.to_categorical(y_test, nb_classes)

# ===== Beginning of model definition =====
print('Build model...')
# Since we have multiple convolutional layers in parallel, 
# Graph is needed instead of Sequential.
model = Graph()

model.add_input(name='input', input_shape=(maxlen,), dtype='int')

# Start off with an efficient embedding layer which maps
# our vocab indices into embedding_dims dimensions
embedding_layer = Embedding(max_features, embedding_dims, input_length=maxlen, 
                            weights=[w2v_word_vec], trainable=True)
model.add_node(embedding_layer, name='embedding', input='input')
model.add_node(Dropout(0.5), name='embedding_dropout', input='embedding')

# Add several Convolutional1D, learning nb_filter word group filters
# with different filter_lengths 
for filter_length in filter_lengths:
    sequential = containers.Sequential()
    sequential.add(Convolution1D(nb_filter=nb_filter,
                        filter_length=filter_length,
                        border_mode='valid',
                        activation='relu',
                        subsample_length=1,
                        input_shape=(maxlen, embedding_dims)))
    sequential.add(MaxPooling1D(pool_length=pool_length))
    sequential.add(Flatten())
    
    model.add_node(sequential, name='unit_'+str(filter_length), input='embedding_dropout')

fc = containers.Sequential()
# Compute the shape of output of convolutional layers
last_output_dim = 0
for filter_length in filter_lengths:
    last_output_dim += nb_filter * int((maxlen - filter_length + 1) / pool_length)
# Add hidden layers:
fc.add(Dense(hidden_dims, input_dim=last_output_dim))
fc.add(ELU())
fc.add(Dropout(0.5))

fc.add(Dense(hidden_dims))
fc.add(Activation('tanh'))
fc.add(Dropout(0.5))

# Softmax
fc.add(Dense(nb_classes))
fc.add(Activation('softmax'))

if len(filter_lengths) > 1:
    model.add_node(fc, name='full_connected', 
        inputs=['unit_' + str(n) for n in filter_lengths], merge_mode='concat')
elif len(filter_lengths) == 1:
    model.add_node(fc, name='full_connected', 
        input='unit_' + str(filter_lengths[0]), merge_mode='concat')

model.add_output(name='output', input='full_connected')

model.compile(loss={'output' : 'categorical_crossentropy'}, optimizer='adam')
# ===== End of model definition =====

# Compute weight per class
# class_weight[i] = Num of examples / Num of examples with label i
class_weight = dict()
nb_examples_per_class = np.sum(Y_train, axis=0)
nb_examples = np.sum(nb_examples_per_class)
for i in range(nb_examples_per_class.shape[0]):
    class_weight[i] = nb_examples * 1.0 / nb_examples_per_class[i]

# Early Stopping
callbacks = [EarlyStopping(monitor='val_loss', patience=2, verbose=0)]

# Fit data
model.fit({'input':X_train, 'output':Y_train}, batch_size=batch_size, 
          nb_epoch=nb_epoch, class_weight=class_weight,
          validation_data={'input':X_test, 'output':Y_test},
          callbacks=callbacks)

predictions = model.predict({'input':X_test})['output']

# Show classification result
result = classifier_eval.eval(Y_test, predictions, True)
