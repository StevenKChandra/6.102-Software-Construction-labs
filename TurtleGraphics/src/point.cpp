#include "point.h"

Point::Point(double x, double y): x_(x), y_(y) {}

double Point::x() const { return x_; }

double Point::y() const { return y_; }
