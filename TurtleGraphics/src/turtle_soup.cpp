#include <iostream>
#include <cmath>

#include "turtle_soup.h"
#include "util.h"

void TurtleSoup::drawSquare(Turtle* turtle, int sideLength) {
    drawRegularPolygon(turtle, 4, sideLength);
}
double TurtleSoup::calculateRegularPolygonAngle(int sides) {
    return (double) 180.0f * double (sides - 2) / (double) sides;
}

int TurtleSoup::calculatePolygonSidesFromAngle(double angle) {
    return util::doubleRound((double) 360.0f / ((double) 180.0f - angle));
}

void TurtleSoup::drawRegularPolygon(Turtle* turtle, int sides, int sideLength) {
    double angle = calculateRegularPolygonAngle(sides);
    for (int i = 0; i < sides; i++) {
        turtle->forward(sideLength);
        turtle->turn(angle);
    }
}

double TurtleSoup::calculateHeadingToPoint(
    double currentHeadingDegree,
    int currentX,
    int currentY,
    int targetX,
    int targetY) 
{
    double deltaX = targetX - currentX;
    double deltaY = targetY - currentY;

    if (deltaX == 0 && deltaY == 0) {
        return 0.0f;
    }

    double angleRadian = std::atan2(deltaX, deltaY);
    double currentHeadingRadian = util::degToRadian(currentHeadingDegree);
    double outputRadian = util::doubleModulo(angleRadian - currentHeadingRadian, 2 * M_PI);
    return util::radianToDeg(outputRadian);
}
std::vector<double> TurtleSoup::calculateHeadings(const std::vector<double>& xCoords, const std::vector<double>& yCoords) {
    if (xCoords.size() != yCoords.size()) {
        throw std::domain_error("xCoords and yCoords have different length!");
    }
    std::vector<double> output{};
    double currentX = xCoords[0];
    double currentY = yCoords[0];
    double currentHeadingDegree = 0;
    for (int i = 1; i < xCoords.size(); i++) {
        double nextX = xCoords[i];
        double nextY = yCoords[i];
        currentHeadingDegree =  calculateHeadingToPoint(currentHeadingDegree, currentX, currentY, nextX, nextY);
        output.push_back(currentHeadingDegree);
        currentX = nextX;
        currentY = nextY;
    }
    return output;
}

void TurtleSoup::main(int argc, char *argv[]) {
    DrawableTurtle turtle = DrawableTurtle();
    
    drawSquare(&turtle, 40);
}