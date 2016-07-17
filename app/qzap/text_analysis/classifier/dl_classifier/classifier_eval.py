#!/usr/bin/env python
#coding:utf8

# Copyright (c) 2015, Tencent.
# All Rights Reserved.
# Author: Xinyan Lu (xinyanlu@tencent.com)

# Evaluation of classifier

from __future__ import print_function
import numpy as np

# Calculate F-score / Precision / Recall / Accuracy
def eval(Y_true, Y_pred, show_category_level = False):
    '''
    Input:
    Y_true: ndarray with shape (num_examples, num_classes)
    Y_pred: ndarray with shape (num_examples, num_classes)
    show_category_level: boolean, whether showing the details of each category

    Note: assuming class 0 as the negative class

    Output:
    tuple: (F_score_positive, Precision_positive, Recall_positive, Accuracy)
    F_score_positive: float
    Precision_positive: float
    Recall_positive: float
    Accuracy: float
    '''
    if len(Y_true) != len(Y_pred):
        raise "Number of examples doesn't match."

    num_classes = Y_true.shape[1]

    # Calculate prediction class
    y_pred_class = np.argmax(Y_pred, axis = 1)
    Y_pred_binary = np.zeros(Y_pred.shape)
    Y_pred_binary[np.arange(Y_pred.shape[0]), y_pred_class.tolist()] = 1

    num_examples = np.sum(Y_true, axis = 0)
    num_pred_examples = np.sum(Y_pred_binary, axis = 0)
    num_match_examples = np.sum(Y_true * Y_pred_binary, axis = 0)
            
    Accuracy = sum(num_match_examples) / sum(num_pred_examples)
    
    Precision = num_match_examples / num_pred_examples
    Recall = num_match_examples / num_examples
    F_score = 2 * Precision * Recall / (Precision + Recall)  
    
    # Sum the positive classes
    Precision_positive = sum(num_match_examples[1:]) / sum(num_pred_examples[1:])
    Recall_positive = sum(num_match_examples[1:]) / sum(num_examples[1:])
    F_score_positive = 2 * Precision_positive * Recall_positive / (Precision_positive + Recall_positive)
    
    print('Accuracy         : %.4f' % (Accuracy,))
    print('')
    print('F-score   (Micro): %.4f' % (F_score_positive,))
    print('Precision (Micro): %.4f' % (Precision_positive,))
    print('Recall    (Micro): %.4f' % (Recall_positive,))
    print('')
    print('F-score   (Macro): %.4f' % (np.mean(F_score[1:]),))
    print('Precision (Macro): %.4f' % (np.mean(Precision[1:]),))
    print('Recall    (Macro): %.4f' % (np.mean(Recall[1:]),))
    
    if show_category_level:
        print(' %3s   %10s   %10s   %8s   %10s   %8s   %9s ' 
                % ('Id', 'Examples', 'Predicted', 'Match', 
                   'Precision', 'Recall', 'F-Score'))
        print('=' * 79)
        for i in range(num_classes):
            print(' %3s   %10d   %10d   %8d   %10.4f   %8.4f   %9.4f ' 
                    % (i, num_examples[i], num_pred_examples[i], num_match_examples[i], 
                       Precision[i], Recall[i], F_score[i])) 
        print('=' * 79)
            
    return (F_score_positive, Precision_positive, Recall_positive, Accuracy)
    