// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
// Author: ManFeng XU <svdxu@tencent.com>

#ifndef APP_QZAP_COMMON_UTILITY_TARGETING_SOLID_FINGER_PRINT_H_
#define APP_QZAP_COMMON_UTILITY_TARGETING_SOLID_FINGER_PRINT_H_

#include <stdint.h>
#include <string>
#include "app/qzap/proto/common/qzap_ad_targeting.pb.h"
//#include "app/qzap/proto/common/qzap_ad_targeting.proto"

void StableTargetingSolid(const QZAP::AD::TargetingSolid& solid, QZAP::AD::TargetingSolid* stable_solid);

uint64_t TargetingSolidFingerPrint(const QZAP::AD::TargetingSolid& solid);

#endif  // APP_QZAP_COMMON_UTILITY_TARGETING_SOLID_FINGER_PRINT_H_




