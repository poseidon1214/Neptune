// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
// Author: ManFeng XU <svdxu@tencent.com>

#include "app/qzap/common/utility/targeting_solid_finger_print.h"

#include <map>

#include "app/qzap/common/base/shared_ptr.h"
#include "thirdparty/glog/logging.h"

#include "app/qzap/common/utility/hash.h"


static void StableTargetingLine(const QZAP::AD::TargetingLine& line, QZAP::AD::TargetingLine* stable_line)
{
    std::map<std::string, const QZAP::AD::TargetingPoint*> point_finger_prints;
    for (int32_t i = 0; i < line.targeting_point_size(); i++)
    {
        std::string finger = line.targeting_point(i).ShortDebugString();
        LOG_IF(WARNING, point_finger_prints.count(finger) != 0u)
            << "duplicate point point=[" << finger << "] line = [" << line.ShortDebugString() << "]";
        point_finger_prints[finger] = &(line.targeting_point(i));
    }
    stable_line->CopyFrom(line);
    stable_line->clear_targeting_point();
    for (std::map<std::string, const QZAP::AD::TargetingPoint*>::const_iterator
           it = point_finger_prints.begin();
         it != point_finger_prints.end();
         ++it)
    {
        stable_line->add_targeting_point()->CopyFrom(*(it->second));
    }
    //CHECK_EQ(line.targeting_point_size(), stable_line->targeting_point_size());
}
static void StableTargetingPlane(const QZAP::AD::TargetingPlane& plane,  QZAP::AD::TargetingPlane* stable_plane)
{
    std::map<std::string, shared_ptr<QZAP::AD::TargetingLine> > line_finger_prints;
    for (int32_t i = 0; i < plane.targeting_line_size(); i++)
    {
        shared_ptr<QZAP::AD::TargetingLine> stable_line(new QZAP::AD::TargetingLine);
        StableTargetingLine(plane.targeting_line(i), stable_line.get());
        std::string finger = stable_line->ShortDebugString();
        LOG_IF(WARNING, line_finger_prints.count(finger) != 0u)
            << "duplicate line line=[" << finger << "] plane = [" << plane.ShortDebugString() << "]";
        line_finger_prints[finger] = stable_line;
    }
    stable_plane->CopyFrom(plane);
    stable_plane->clear_targeting_line();
    for (std::map<std::string, shared_ptr<QZAP::AD::TargetingLine> >::const_iterator
           it = line_finger_prints.begin();
         it != line_finger_prints.end();
         ++it)
    {
        stable_plane->add_targeting_line()->CopyFrom(*(it->second.get()));
    }
    //CHECK_EQ(plane.targeting_line_size(), stable_plane->targeting_line_size());

}
void StableTargetingSolid(const QZAP::AD::TargetingSolid& solid, QZAP::AD::TargetingSolid* stable_solid)
{
    std::map<std::string, shared_ptr<QZAP::AD::TargetingPlane> > plane_finger_prints;
    for (int32_t i = 0; i < solid.targeting_plane_size(); i++)
    {
        shared_ptr<QZAP::AD::TargetingPlane> stable_plane(new QZAP::AD::TargetingPlane);
        StableTargetingPlane(solid.targeting_plane(i), stable_plane.get());
        std::string finger = stable_plane->ShortDebugString();
        LOG_IF(WARNING, plane_finger_prints.count(finger) != 0u)
            << "duplicate plane plane=[" << finger << "] solid = [" << solid.ShortDebugString() << "]";
        plane_finger_prints[finger] = stable_plane;
    }
    stable_solid->CopyFrom(solid);
    stable_solid->clear_targeting_plane();
    for (std::map<std::string, shared_ptr<QZAP::AD::TargetingPlane> >::const_iterator
           it = plane_finger_prints.begin();
         it != plane_finger_prints.end();
         ++it)
    {
        stable_solid->add_targeting_plane()->CopyFrom(*(it->second.get()));
    }
    //CHECK_EQ(solid.targeting_plane_size(), stable_solid->targeting_plane_size());

}
uint64_t TargetingSolidFingerPrint(const QZAP::AD::TargetingSolid& solid)
{
    QZAP::AD::TargetingSolid  stable_solid;
    StableTargetingSolid(solid, &stable_solid);
    return hash_string(stable_solid.ShortDebugString());
}

