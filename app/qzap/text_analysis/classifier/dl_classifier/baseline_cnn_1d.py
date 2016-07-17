#!/usr/bin/env python
#coding:utf8

# Copyright (c) 2015, Tencent.
# All Rights Reserved.
# Author: Xinyan Lu (xinyanlu@tencent.com)

# A baseline convolutional neural network
# Network structure:
# sequence input(length=150) -> embedding (dim=64, randomly initialized)
#                            -> conv1D (num_filters=250, length=3) 
#                            -> maxpooling (pool=2) 
#                            -> full-connected (hidden_unit=250) 
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
# Epoch 1/3
# 208034/208034 [==============================] - 137s - loss: 0.7812 - val_loss: 0.5868
# Epoch 2/3
# 208034/208034 [==============================] - 138s - loss: 0.3042 - val_loss: 0.5288
# Epoch 3/3
# 208034/208034 [==============================] - 137s - loss: 0.2259 - val_loss: 0.5248
# Accuracy         : 0.8662
# F-score   (Micro): 0.8420
# Precision (Micro): 0.8746
# Recall    (Micro): 0.8117
# F-score   (Macro): 0.7838
# Precision (Macro): 0.8355
# Recall    (Macro): 0.7445
#   Id     Examples    Predicted      Match    Precision     Recall     F-Score 
# ===============================================================================
#    0         5615         6136       5258       0.8569     0.9364      0.8949 
#    1         1068         1117        945       0.8460     0.8848      0.8650 
#    2          241          220        185       0.8409     0.7676      0.8026 
#    3          450          427        398       0.9321     0.8844      0.9076 
#    4          322          304        285       0.9375     0.8851      0.9105 
#    5          278          294        254       0.8639     0.9137      0.8881 
#    6          197          192        152       0.7917     0.7716      0.7815 
#    7           60           37         31       0.8378     0.5167      0.6392 
#    8          172          150        116       0.7733     0.6744      0.7205 
#    9         1306         1238       1116       0.9015     0.8545      0.8774 
#   10         1096          954        846       0.8868     0.7719      0.8254 
#   11           52           51         36       0.7059     0.6923      0.6990 
#   12          396          374        311       0.8316     0.7854      0.8078 
#   13          199          171        151       0.8830     0.7588      0.8162 
#   14          152          119         87       0.7311     0.5724      0.6421 
#   15           91           58         47       0.8103     0.5165      0.6309 
#   16         1080          945        868       0.9185     0.8037      0.8573 
#   17           78           66         47       0.7121     0.6026      0.6528 
# ===============================================================================

from __future__ import print_function
import cPickle
import numpy as np

np.random.seed(1337)  # for reproducibility

from keras.preprocessing import sequence
from keras.layers.core import Dense, Dropout, Activation, Flatten
from keras.layers.embeddings import Embedding
from keras.layers.convolutional import Convolution1D, MaxPooling1D
from keras.models import Sequential
from keras.utils import np_utils

import classifier_eval

# Retain the first *maxlen* words in a document
maxlen = 150
# Retain the top *max_features* words with highest occurrences
max_features = 50000
# Training batch size
batch_size = 64
# Embedding size of each word
embedding_dims = 64
# Number of convolutional filters
nb_filter = 250
# Length of convolutional filters
filter_length = 3
# Number of hidden units in the full-connected layer
hidden_dims = 250
# Training epoches
nb_epoch = 3

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
model = Sequential()

# Start off with an efficient embedding layer which maps
# our vocab indices into embedding_dims dimensions
model.add(Embedding(max_features, embedding_dims, input_length=maxlen))
model.add(Dropout(0.5))

# Add a Convolution1D, which will learn nb_filter
# word group filters of size filter_length:
model.add(Convolution1D(nb_filter=nb_filter,
                        filter_length=filter_length,
                        border_mode='valid',
                        activation='relu',
                        subsample_length=1))
# Use standard max pooling (halving the output of the previous layer):
model.add(MaxPooling1D(pool_length=2))

# Flatten the output of the conv layer,
# so that we can add a vanilla dense layer:
model.add(Flatten())

# Add a vanilla hidden layer:
model.add(Dense(hidden_dims))
model.add(Dropout(0.5))
model.add(Activation('relu'))

# Softmax
model.add(Dense(nb_classes))
model.add(Activation('softmax'))

model.compile(loss='categorical_crossentropy', optimizer='adam')
# ===== End of model definition =====

# Fit data
model.fit(X_train, Y_train, batch_size=batch_size, nb_epoch=nb_epoch, 
          validation_data=(X_test, Y_test))

predictions = model.predict(X_test)

# Show classification result
result = classifier_eval.eval(Y_test, predictions, True)
