#pragma once

#include <cmath>

inline int float_cmp(float a, float b, float epsilon = 0.000001f) {
    if (fabsf(a-b) < epsilon) return 0;
    else return a < b ? -1 : 1;
}

inline int double_cmp(double a, double b, double epsilon = 0.000001) {
    if (fabs(a-b) < epsilon) return 0;
    else return a < b ? -1 : 1;
}