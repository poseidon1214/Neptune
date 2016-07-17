// Copyright (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2013-03-12
#ifndef APP_QZAP_COMMON_UTILITY_INTERVAL_H_
#define APP_QZAP_COMMON_UTILITY_INTERVAL_H_
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>

template <typename T>
class Interval {
 public:
  // Instantiates an Empty() interval:
  Interval()
    : min_(), max_() {
  }

  // The interval spans [min, max); note the interval does not contain
  // the value <max>!
  Interval(const T &min, const T &max)
    : min_(min), max_(max) {
  }

  // Note that if the interval is Empty(), it is possible for max() to be less
  // than min().
  const T& min() const {
    return min_;
  }

  const T& max() const {
    return max_;
  }

  T Center() const {
    return (min_ + max_) / (T)2;
  }

  void SetMin(const T& t) {
    min_ = t;
  }
  void SetMax(const T& t) {
    max_ = t;
  }
  void Set(const T& min, const T& max) {
    min_ = min;
    max_ = max;
  }

  // Sets the interval to be Empty() again.
  void Clear() {
    min_ = max_ = T();
  }

  // Updates <this> to have the same range as <i>:
  void CopyFrom(const Interval<T> &i) {
    min_ = i.min_;
    max_ = i.max_;
  }

  float Range() const {
    return fabs(max_ - min_);
  }

  // Returns true iff <this> and <i> are non-Empty() and both
  // intervals have the same endpoints, or if both intervals
  // are Empty():
  bool Equals(const Interval<T> &i) const {
    return ((!Empty() && !i.Empty() && min_ == i.min_ && max_ == i.max_) ||
            (Empty() && i.Empty()));
  }

  // Returns true iff this interval Contains() no values.
  // If <min> == <max>, the interval is considered to be Empty():
  bool Empty() const {
    return (min_ >= max_);
  }

  // Returns true iff <t> is >= min() and < max():
  bool Contains(const T& t) const {
    return (min_ <= t && max_ > t);
  }

  // Returns true iff <this> and <i> are non-empty, and every T
  // in <i> is also Contained() in <this>:
  bool Contains(const Interval<T> &i) const {
    return (!Empty() && !i.Empty() && min_ <= i.min_ && max_ >= i.max_);
  }

  // Returns true iff <this> and <i> are non-Empty() and
  // <i> intersects <this> interval:
  bool Intersects(const Interval<T> &i) const {
    return (!Empty() && !i.Empty() && min_ < i.max_ && max_ > i.min_);
  }

  // Returns true iff <this> and <i> are non-Empty() and <i>
  // intersects <this> interval. If <xit> is the intersection of
  // <this> and <i>, and <intersection> is not the null pointer, <xit>
  // is placed in <intersection>:
  bool Intersects(const Interval<T> &i, Interval<T>* intersection) const {
    if (Intersects(i)) {
      if (intersection != NULL) {
        intersection->min_ = std::max(min_, i.min_);
        intersection->max_ = std::min(max_, i.max_);
      }
      return true;
    } else {
      return false;
    }
  }

  // Sets <this> to be the intersection of itself with <i>.  Returns true iff
  // <this> was modified:
  bool IntersectWith(const Interval<T> &i) {
    if (Empty()) {
      return false;
    } else {
      bool modified = false;
      if (i.min_ > min_) {
        min_ = i.min_;
        modified = true;
      }
      if (i.max_ < max_) {
        max_ = i.max_;
        modified = true;
      }
      return modified;
    }
  }

  // If <i> is non-Empty(), sets the range of <this> so that it represents
  // the smallest interval that contains both <this> and <i>. Returns true
  // iff <this> was modified:
  bool SpanningUnion(const Interval<T> &i) {
    if (i.Empty()) {
      return false;
    } else if (Empty()) {
      min_ = i.min_;
      max_ = i.max_;
      return true;
    } else {
      bool modified = false;
      if (i.min_ < min_) {
        min_ = i.min_;
        modified = true;
      }
      if (i.max_ > max_) {
        max_ = i.max_;
        modified = true;
      }
      return modified;
    }
  }

  // If <xit> is the intersection of <this> and <i>, computes the
  // intervals resulting from (<this> - <xit>) and appends them
  // to <difference>. Returns false iff the intersection is empty.
  // The caller takes ownership of the returned intervals.
  bool Difference(const Interval<T> &i,
                  std::vector<Interval<T>*> *difference) const {
    if (Empty()) {
      // <empty> - <i> = <empty>
      return false;
    }
    if (i.Empty()) {
      // <this> - <empty> = <this>
      difference->push_back(new Interval<T>(min_, max_));
      return false;
    }
    // <this> - <Empty> = <this>
    if (min_ < i.max_ && min_ >= i.min_ && max_ > i.max_) {
      //            [------ this ------]
      // [------ i ------]
      difference->push_back(new Interval<T>(i.max_, max_));
      return true;
    } else if (max_ > i.min_ && max_ <= i.max_ && min_ < i.min_) {
      // [------ this ------]
      //            [------ i ------]
      difference->push_back(new Interval<T>(min_, i.min_));
      return true;
    } else if (min_ < i.min_ && max_ > i.max_) {
      // [------ this ------]
      //    [---- i ----]
      difference->push_back(new Interval<T>(min_, i.min_));
      difference->push_back(new Interval<T>(i.max_, max_));
      return true;
    } else if (min_ >= i.min_ && max_ <= i.max_) {
      // [------ i --------]
      //   [--- this ---]
      // Intersection is <this>, so difference yields the empty interval.
      // Push nothing into <difference>:
      return true;
    } else {
      // No intersection. Append <this>:
      difference->push_back(new Interval<T>(min_, max_));
      return false;
    }
  }

  // Computes the difference of <this> and <i>, which is made up of at most two
  // intervals: one aligned at the low end of <this> (which will be stored in
  // *lo) and one aligned at the high end of <this> (which will be stored in
  // *hi).  If <i> and <this> are disjoint, then, by convention, we set *lo =
  // <this> and *hi = <empty>. Returns false iff the intersection of <i> and
  // <this> is empty.
  bool Difference(const Interval<T> &i,
                  Interval<T> *lo, Interval<T> *hi) const {
    // Initialize *lo and *hi to empty
    *lo = Interval<T>();
    *hi = Interval<T>();
    if (Empty())
      return false;
    if (i.Empty()) {
      *lo = *this;
      return false;
    }
    if (min_ < i.max_ && min_ >= i.min_ && max_ > i.max_) {
      //            [------ this ------]
      // [------ i ------]
      *hi = Interval<T>(i.max_, max_);
      return true;
    } else if (max_ > i.min_ && max_ <= i.max_ && min_ < i.min_) {
      // [------ this ------]
      //            [------ i ------]
      *lo = Interval<T>(min_, i.min_);
      return true;
    } else if (min_ < i.min_ && max_ > i.max_) {
      // [------ this ------]
      //    [---- i ----]
      *lo = Interval<T>(min_, i.min_);
      *hi = Interval<T>(i.max_, max_);
      return true;
    } else if (min_ >= i.min_ && max_ <= i.max_) {
      // [------ i --------]
      //   [--- this ---]
      // Intersection is <this>, so difference yields the empty interval.
      return true;
    } else {
      // No intersection:
      *lo = Interval<T>(min_, max_);
      return false;
    }
  }

  // Comparator, for storing Intervals into a set
  struct Less {
    bool operator()(const Interval<T> &a, const Interval<T> &b) const {
      // This ordering is hacky, but it guarantees that, if interval X is
      // included in Y, then Y <= X
      return (a.min() < b.min() ||
              (a.min() == b.min() && a.max() > b.max()));
    }
  };

  const std::string DebugString() const {
    std::ostringstream os;
    os << "[" << this->min() << ", " << this->max() << ")";
    return os.str();
  }

 private:
  T min_;
  T max_;
};
#endif  // APP_QZAP_COMMON_UTILITY_INTERVAL_H_
