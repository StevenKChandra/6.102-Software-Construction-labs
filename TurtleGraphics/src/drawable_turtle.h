#ifndef DRAWABLE_TURTLE_H
#define DRAWABLE_TURTLE_H

#include <vector>

#include "turtle.h"
#include "action.h"

#define CANVAS_WIDTH 512
#define CANVAS_HEIGHT 512

#define CIRCLE_DEGREES 360.0f
#define DEGREES_TO_VERTICAL 90

#define DEG_TO_RAD(d) d/180.0f * M_PI

const std::string penColorToString[] {
    "BLACK",
    "BLUE",
    "CYAN",
    "DARK_GRAY",
    "GRAY",
    "GREEN",
    "LIGHT_GRAY",
    "MAGENTA",
    "ORANGE",
    "PINK",
    "RED",
    "WHITE",
    "YELLOW"
};

class DrawableTurtle: public Turtle {
public:
    /**
     * Constructs a new DrawableTurtle.
     */
    DrawableTurtle();

    /**
     * Moves the turtle forward by the given distance.
     *
     * @param distance the distance to move
     */
    void forward(double distance) override;

    /**
     * Turns the turtle left by the given angle.
     *
     * @param angle the angle to turn
     */
    virtual void turn(double angle) override;

    /**
     * Changes the turtle's current pen color.
     *
     * @param color the new pen color
     */
    void color(PenColor color) override;

private:
    std::vector<Action> actionList;
    std::vector<LineSegment> lines;
    
    Point currentPosition;
    double currentHeading;
    PenColor currentColor;
};

#endif