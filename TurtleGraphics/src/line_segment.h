#ifndef TURTLE_LINESEGMENT_H
#define TURTLE_LINESEGMENT_H

#include "point.h"
#include "pen_color.h"

class LineSegment {
public:
    /**
     * Constructs a new LineSegment from coordinate pairs.
     * 
     * @param startx x-coordinate of start point
     * @param starty y-coordinate of start point
     * @param endx x-coordinate of end point
     * @param endy y-coordinate of end point
     * @param color line segment color
     */
    LineSegment(double startX, double startY, double endX, double endY, PenColor color);

    /**
     * Constructs a new LineSegment from start and end points.
     *
     * @param start the start point
     * @param end the end point
     */
    LineSegment(const Point& start, const Point& end, const PenColor& color);

    /**
     * Returns the start point of this line segment.
     *
     * @return the start point
     */
    Point start();

    /**
     * Returns the end point of this line segment.
     *
     * @return the end point
     */
    Point end();

    /**
     * Returns the color of the line segment
     * 
     * @return color of the line segment
     */
    PenColor color();

    /**
     * Compute the length of this segment.
     * 
     * @return the length of the line segment
     */
    double length();

private:
    Point start_;
    Point end_;
    PenColor pencolor;
};

#endif