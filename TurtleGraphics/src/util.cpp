#include "util.h"

#define M_PI 3.14159265358979323846f

double util::doubleModulo(double a, double b) {
    if (b == 0.0f) return a;
    
    if (b < 0) {
        b = -b;
    }
    
    while (a >= b) {
        a = a - b;
    }
    while (a < 0) {
        a = a + b;
    }
    return a;
}

int util::doubleRound(double d) {
    return (int) (d + 0.5f);
}

double util::radianToDeg(double rad) {
    return rad / M_PI * 180.0f;
}

double util::degToRadian(double deg) {
    return deg / 180.0f * M_PI;
}