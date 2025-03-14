#include <cmath>
#include "line_segment.h"

LineSegment::LineSegment(double startX, double startY, double endX, double endY, PenColor color):
    start_{startX, startY}, end_{endX, endY}, pencolor(color) {};

LineSegment::LineSegment(const Point& start, const Point& end, const PenColor& color):
    start_(start), end_(end), pencolor(color) {};

Point LineSegment::start() { return start_; }

Point LineSegment::end() { return end_; }

PenColor LineSegment::color() { return pencolor; }

double LineSegment::length() {
    return std::sqrt((start_.x() - end_.x()) * (start_.x() - end_.x()) +
        (start_.y() - end_.y()) * (start_.y() - end_.y()));
}