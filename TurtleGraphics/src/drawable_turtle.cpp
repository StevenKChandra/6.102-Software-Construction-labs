#include <cmath>
#include "util.h"
#include "drawable_turtle.h"

DrawableTurtle::DrawableTurtle():
    currentPosition{0.0f, 0.0f}, currentHeading{0.0f}, currentColor{PenColor::BLACK} { }

void DrawableTurtle::forward(double distance) {
    double nextX = std::sin(DEG_TO_RAD(currentHeading)) * distance + currentPosition.x();
    double nextY = std::cos(DEG_TO_RAD(currentHeading)) * distance + currentPosition.y();
    
    lines.push_back(LineSegment{currentPosition.x(), currentPosition.y(), nextX, nextY, currentColor});
    actionList.push_back(Action{ActionType::FORWARD, "move forward " + std::to_string(distance) + "steps"});
}

void DrawableTurtle::turn(double angle)  {
    currentHeading += angle;
    currentHeading = util::doubleModulo(currentHeading, CIRCLE_DEGREES);
    actionList.push_back(Action{ActionType::TURN, "turn" + std::to_string(angle) + "degrees"});
}

void DrawableTurtle::color(PenColor color) {
    currentColor = color;
    actionList.push_back(Action{ActionType::COLOR, "change color to " + (penColorToString[color])});
}