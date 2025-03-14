#ifndef TURTLE_TURTLE_H
#define TURTLE_TURTLE_H

#include "point.h"
#include "pen_color.h"

/**
 * Turtle interface.
 * 
 * Defines the interface that any turtle must implement. Note that the
 * standard directions/rotations use Logo semantics: initial heading
 * of zero is 'up', and positive angles rotate the turtle clockwise.
 */

class Turtle {
public:
    /**
     * Moves the turtle forward by the given distance.
     *
     * @param distance the distance to move
     */
    virtual void forward(double distance) = 0;

    /**
     * Turns the turtle left by the given angle.
     *
     * @param angle the angle to turn
     */
    virtual void turn(double angle) = 0;

    /**
     * Change the turtle's current pen color.
     * 
     * @param color new pen color
     */
    virtual void color(PenColor color) = 0;
};

#endif