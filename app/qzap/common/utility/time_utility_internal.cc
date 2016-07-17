// Copyright (C), 1998-2013, Tencent
// Author: jefftang@tencent.com
// Date: 2013-3-5
#include "app/qzap/common/utility/time_utility.h"
#include "app/qzap/common/utility/time_utility_internal.h"
void GetNthPreviousDayCarefully(int n, int *y, int *m, int *d) {
  *d -= n;
  while ((*d) < 1) {
    --(*m);
    if ((*m) < 1) {
      (*m) = 12;
      --(*y);
    }
    (*d) += GetDaysInMonth(*y, *m);
  }
}

void GetNthNextDayCarefully(int n, int *y, int *m, int *d) {
  *d += n;
  while ((*d) > GetDaysInMonth(*y, *m)) {
    (*d) -= GetDaysInMonth(*y, *m);
    ++(*m);
    if ((*m) > 12) {
      (*m) = 1;
      ++(*y);
    }
  }
}
