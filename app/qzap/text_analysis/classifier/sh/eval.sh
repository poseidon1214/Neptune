#!/bin/bash

# Copyright (c) 2013 Tencent Inc.
# Author: Lifeng Wang (fandywang@tencent.com)

usage()
{
    echo "Usage: `basename $0` classifier_model test_data_file"
    exit 1
}
if [ $# -ne 2 ]
then
    usage
fi

classifier_model=$1
test_data_file=$2

if [ ! -d $classifier_model ]; then
    echo "error: classifier model directory '$classifier_model' does not exist.\n"
    usage
fi
if [ ! -f $test_data_file ]; then
    echo "error: test data file '$test_data_file' does not exist.\n"
    usage
fi


echo "分类器模型评测 classifier evaluation ..."

time ./classifier_evaluator_main \
    --classifier_model_dir=${classifier_model} \
    --segmenter_data_dir=tc_data \
    --text_miner_resource_config_file=text_miner_resource.config \
    --topk_results=1 \
    --on_token_feature=true \
    --on_keyword_feature=false \
    --on_lda_feature=true \
    --on_embedding_feature=false \
    --test_corpus_file=${test_data_file} \
    --evaluation_result_file=test.eval.result \
    --test_corpus_output_file=test.predict.result
echo "done"
