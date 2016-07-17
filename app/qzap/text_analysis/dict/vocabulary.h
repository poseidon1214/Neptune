// Copyright 2013 Tencent Inc.
// Author: Lifeng Wang (fandywang@tencent.com)
//
// 词典类
// 1、Build生成二进制词典文件，方便其他模块Load使用，避免模块读取多个文本文件
// 2、提供词的查找功能，查找是否存在，根据word查找index，根据index查找word，
//    不支持修改和添加word

#ifndef APP_QZAP_TEXT_ANALYSIS_VOCABULARY_H_
#define APP_QZAP_TEXT_ANALYSIS_VOCABULARY_H_

#include <string>
#include <vector>

#include "common/base/uncopyable.h"
#include "app/qzap/text_analysis/thirdparty/darts.h"

namespace qzap {
namespace text_analysis {

class Vocabulary {
 public:
  Vocabulary();
  virtual ~Vocabulary();
  virtual void Clear();

  // 输入纯文本文件，每行一个独立词条，构建词典对象
  virtual bool Build(const std::string& filename);

  // 以二进制文件输出
  virtual bool Save(const std::string& filename) const;

  // 加载二进制词典
  virtual bool Load(const std::string& filename);

  // 词是否在词典中
  bool HasWord(const std::string& word) const;

  // 返回词的index，如果不存在返回-1
  int WordIndex(const std::string& word) const;

  // 由index获取词，注意：如果index越界，将会 core dump(segment fault)
  const std::string& Word(int index) const;

  // 获取词的总数
  int Size() const;

  // 记录匹配子串在原始文本中的位置
  struct ExtractResultType {
    std::vector<size_t> boundaries;
  };

  // 抽取 text 中出现在 Vocabulary 中的所有子串，此处按照字节进行匹配查找，
  // 如 Vocabulary = {流行，新流行，时尚发型，美女图片}
  //    text = "最新流行时尚发型图片"
  //    results = {新流行，流行，时尚发型}
  //
  // text：输入参数，待匹配的原始文本
  // results：输出参数，抽取结果集合
  bool ExtractByByte(const std::string& text,
                     std::vector<ExtractResultType>* results) const;

  // 抽取 text 中出现在 Vocabulary 中的所有子串，此处按照 token 进行匹配查找，
  // 如 Vocabulary = {流行，新流行，时尚发型，美女图片}
  //    text = "最新流行时尚发型图片",
  //    text分词结果为 "最新 流行 时尚 发型 图片"
  //    results = {流行，时尚发型}
  //
  // text：输入参数，待匹配的原始文本
  // token_boundaries: 输入参数，text 的分词边界
  // results：输出参数，抽取结果集合
  bool ExtractByToken(const std::string& text,
      const std::vector<size_t>& token_boundaries,
      std::vector<ExtractResultType>* results) const;

 protected:
  Darts::DoubleArray dict_;  // 使用 darts 数据结构组织词典
  std::vector<std::string> index_to_word_;  // 存储词条word，可以根据index获取
                                            // word，index 与 darts 中的 value
                                            // 一一对应

  DECLARE_UNCOPYABLE(Vocabulary);
};

}  // namespace text_analysis
}  // namespace qzap

#endif  // APP_QZAP_TEXT_ANALYSIS_VOCABULARY_H_
