#!/bin/bash

# Copyright (c) 2013 Tencent Inc.
# Author: Lifeng Wang (fandywang@tencent.com)

usage()
{
    echo "Usage: `basename $0` train_data_file"
    exit 1
}
if [ $# -ne 1 ]
then
    usage
fi

train_data_file=$1

if [ ! -f $train_data_file ]; then
    echo "error: training data file '$train_data_file' does not exist.\n"
    usage
fi

echo "分类器模型训练 classifier training ..."
echo "1. 生成特征词典  gen_feature_vocabulary ..."
time ./gen_feature_vocabulary \
    --segmenter_data_dir=tc_data/ \
    --text_miner_resource_config_file=text_miner_resource.config \
    --train_data_file=${train_data_file} \
    --feature_vocabulary_file=feature_vocabulary.dat
echo "done"

echo "2. 特征词典编码成二进制文件 build_vocabulary ..."
time ./build_vocabulary \
    --input_path=feature_vocabulary.dat \
    --output_file=feature.vocabulary
echo "done"

echo "3. 特征提取  feature_extractor_main ..."
time ./feature_extractor_main \
    --segmenter_data_dir=tc_data/ \
    --text_miner_resource_config_file=text_miner_resource.config \
    --train_data_file=${train_data_file} \
    --feature_vocabulary_file=feature.vocabulary \
    --on_token_feature=true \
    --on_keyword_feature=false \
    --on_lda_feature=true \
    --on_embedding_feature=false \
    --train_feature_file=train_feature.dat
echo "done"

echo "4. 模型训练  hierarchical_classifier_trainer ..."
time ./hierarchical_classifier_trainer \
    --taxonomy_file=taxonomy \
    --sample_file=train_feature.dat \
    --model_dir=classifier_model \
    --maxent_training_regularizer_type=L1 \
    --maxent_training_l1_regularizer=0.057
echo "done"
mv feature.vocabulary classifier_model
