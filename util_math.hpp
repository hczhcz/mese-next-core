#pragma once

#include <cmath>

namespace mese {

inline double div(double a, double b, double error) {
    return b == 0 ? error : a / b;
}

inline double abs(double a) {
    return a > 0 ? a : - a;
}

inline double min(double a, double b) {
    return a > b ? b : a;
}

inline double max(double a, double b) {
    return a > b ? a : b;
}

}
