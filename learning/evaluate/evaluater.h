// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: Qian Wang <cernwang@tencent.com>

#ifndef LEARNING_EVALUATE_EVALUATER_H_
#define LEARNING_EVALUATE_EVALUATER_H_

#include <string>
#include <list>
#include <algorithm>
#include "thirdparty/glog/logging.h"
#include "learning/evaluate/mine/cppmine.h"
#include "learning/proto/evaluater_config.pb.h"

namespace gdt {
namespace learning {

template <class T, class ReturnType>
struct Compare {
  //
  ReturnType (T::*GetValue)() const;

 public:
  //
  bool operator ()(const T& a, const T& b) {
    return (a.*(GetValue))() >= (b.*(GetValue))();
  }
  //
  bool operator ()(const std::pair<T*, uint64_t>& a,
                   const std::pair<T*, uint64_t>& b) {
    return (a.first->*(GetValue))() >= (b.first->*(GetValue))();
  }
  //
  bool operator ()(const std::pair<T, uint64_t>& a,
                   const std::pair<T, uint64_t>& b) {
    return (a.first.*(GetValue))() >= (b.first.*(GetValue))();
  }
  //
  Compare(ReturnType (T::*get_value)() const) {
    GetValue = get_value;
  }
};

class Evaluater {
  public:
    // 最大互信息增益  Reference: Original MINE paper: DOI: 10.1126/science.1205438;
    template <class T, class LeftReturnType, class RightReturnType>
    bool MutualInformationCoff(std::vector<T>* objs,
                               LeftReturnType (T::*LeftGetValue)() const,
                               RightReturnType (T::*RightGetValue)() const,
                               EvaluateResult* result) {
      try {
        double *x, *y;
        MINE *mine;
        mine = new MINE(0.6, 15, EST_MIC_APPROX);  
        /* build the problem */
        size_t n = objs->size();
        x = new double [n];
        y = new double [n];
        for (size_t i = 0; i < n; i++) {
          x[i] = ((*objs)[i].*(LeftGetValue))();      
          y[i] = ((*objs)[i].*(RightGetValue))();
        }      
        /* compute score with exceptions management */
        mine->compute_score(x, y, n);
        result->set_mic(mine->mic());
        result->set_mas(mine->mas());
        result->set_mev(mine->mev());
        result->set_mcn(mine->mcn(0));
        result->set_mcn_general(mine->mcn_general());
        delete mine;    
        delete [] x;
        delete [] y;
        return true;
      } catch (char *s) {
        LOG(ERROR) << "Error:" << s;
        return false;
      }
    }
    // ROC曲线下面积 Reference: https://en.wikipedia.org/wiki/Receiver_operating_characteristic#Area_under_curve
    template <class T, class ReturnType>
    double AucCompute(std::vector<T>* objs,
                      ReturnType (T::*GetScore)() const,
                      bool (T::*GroundTruth)() const) {
      Compare<T, ReturnType> compare(GetScore);
      // 按照score排序
      std::sort(objs->begin(), objs->end(), compare);
      // negative_num 正例数, positive_num 负例数
      uint64_t negative_num = 0, positive_num = 0, rank_sum = 0;
      for (uint64_t i = 0; i < objs->size(); i++) {
        if (((*objs)[i].*(GroundTruth))()) {
          positive_num++;
          rank_sum += objs->size() - i;
        } else {
          negative_num++;
        }
      }
      return static_cast<double>(rank_sum - (positive_num * (positive_num + 1) / 2)) / (positive_num * negative_num);
    }
    // ROC曲线下面积 Reference: https://en.wikipedia.org/wiki/Receiver_operating_characteristic#Area_under_curve
    template <class T>
    double AucCompute(std::vector<T>* objs,
                      bool (*ScoreCompare)(const T&, const T&),
                      bool (*GroundTruth)(const T&)) {
      // 按照score排序
      std::sort(objs->begin(), objs->end(), ScoreCompare);
      // negative_num 正例数, positive_num 负例数
      uint64_t negative_num = 0, positive_num = 0, rank_sum = 0;
      for (uint64_t i = 0; i < objs->size(); i++) {
        if ((*GroundTruth)((*objs)[i])) {
          positive_num++;
          rank_sum += objs->size() - i;
        } else {
          negative_num++;
        }
      }
      return static_cast<double>(rank_sum - (positive_num * (positive_num + 1) / 2)) / (positive_num * negative_num);
    }
    // 斯皮尔曼相关性 
    template <class T, class LeftReturnType, class RightReturnType>
    double SpearmanRank(std::vector<T>* objs,
                        LeftReturnType (T::*LeftGetValue)() const,
                        RightReturnType (T::*RightGetValue)() const) {
      uint64_t differential = 0;
      Compare<T, LeftReturnType> left_compare(LeftGetValue);
      Compare<T, RightReturnType> right_compare(RightGetValue);
      std::sort(objs->begin(), objs->end(), left_compare);
      // std::vector<std::pair<T*, uint64_t> > pairs;
      std::vector<std::pair<T, uint64_t> > pairs;
      for (uint64_t i = 0; i < objs->size(); i++) {
        // pairs.push_back(std::make_pair(&((*objs)[i]), i));
        pairs.push_back(std::make_pair((*objs)[i], i));
      }
      std::sort(pairs.begin(), pairs.end(), right_compare);
      for (uint64_t i = 0; i < pairs.size(); i++) {
        int64_t d = int64_t(i) - int64_t(pairs[i].second);
        differential += d * d;
      }
      return 1 - 6 * double(differential) / (pow(double(objs->size()), int(3)) - objs->size());
    }
    // 皮尔逊相关性 
    template <class T, class LeftReturnType, class RightReturnType>
    double PearsonCoefficient(std::vector<T>* objs,
                              LeftReturnType (T::*LeftGetValue)() const,
                              RightReturnType (T::*RightGetValue)() const) {
      double sum_x = 0, sum_y = 0, sum_xx = 0, sum_yy = 0, sum_xy = 0;
      for (uint64_t i = 0; i < objs->size(); i++) {
        sum_x += ((*objs)[i].*(LeftGetValue))();
        sum_y += ((*objs)[i].*(RightGetValue))();
        sum_xy += ((*objs)[i].*(RightGetValue))() * ((*objs)[i].*(LeftGetValue))();
        sum_xx += ((*objs)[i].*(LeftGetValue))() * ((*objs)[i].*(LeftGetValue))();
        sum_yy += ((*objs)[i].*(RightGetValue))() * ((*objs)[i].*(RightGetValue))();
      }
      uint64_t n = objs->size();
      return (sum_xy - (sum_x * sum_y) / n) / sqrt(sum_xx - (sum_x * sum_x) / n) / sqrt (sum_yy - (sum_y * sum_y) / n);
    }
};

}  // namespace learning
}  // namespace gdt

#endif  // LEARNING_EVALUATE_EVALUATER_H_
