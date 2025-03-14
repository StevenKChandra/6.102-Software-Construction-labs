#ifndef TURTLE_POINT_H
#define TURTLE_POINT_H

class Point {
public:
    /**
     * Constructs a new Point with the given x and y coordinates.
     *
     * @param x the x-coordinate
     * @param y the y-coordinate
     */
    Point(double x, double y);

    /**
     * Returns the x-coordinate of this point.
     *
     * @return the x-coordinate
     */
    double x() const;

    /**
     * Returns the y-coordinate of this point.
     *
     * @return the y-coordinate
     */
    double y() const;

private:
    double x_;
    double y_;
};

#endif