// Copyright (c) 2015 Tencent Inc.
// Author: Wang Qian (cernwang@tencent.com)

#ifndef RETRIEVAL_EXTRACTOR_EXTRACTOR_REGISTER_H_
#define RETRIEVAL_EXTRACTOR_EXTRACTOR_REGISTER_H_

#include <map>
#include <utility>
#include <vector>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>

#include "app/qzap/common/base/base.h"
#include "common/base/class_register.h"
#include "data_collector/feeder/framework/data_message.h"
#include "retrieval/extractor/data_message.h"
#include "retrieval/extractor/read_data_functor.h"
#include "retrieval/extractor/write_data_functor.h"
#include "retrieval/extractor/extract_functor.h"

namespace gdt {
namespace common {

typedef ExtractorBaseFunctor Functor;

CLASS_REGISTER_DEFINE_REGISTRY(Functor_register, Functor);
REGISTER_FUNCTOR(ReadDataFunctor, Functor);
REGISTER_FUNCTOR(WriteDataFunctor, Functor);
REGISTER_FUNCTOR(ExtractFunctor, Functor);

}  // namespace common
}  // namespace gdt

#endif  // RETRIEVAL_EXTRACTOR_EXTRACTOR_REGISTER_H_
