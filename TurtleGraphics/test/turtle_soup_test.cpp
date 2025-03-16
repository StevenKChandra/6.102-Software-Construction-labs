#include "gtest/gtest.h"

#include "turtle_soup.h"

namespace {
class TurtleSoupTest: public ::testing::Test {
protected:
    void SetUp(void)
    {

    }

    void TearDown(void)
    {

    }
    DrawableTurtle t;
    TurtleSoup ts;
};

TEST_F(TurtleSoupTest, TestConstructor) {
    ASSERT_NO_THROW(TurtleSoup());
}

TEST_F(TurtleSoupTest, TestPoligonAngleFromSide) {
    EXPECT_NEAR(60.0, ts.calculateRegularPolygonAngle(3), 0.01f) << "FAILURE CALCULATING REGULAR TRIANGLE ANGLE";
    EXPECT_NEAR(128.57, ts.calculateRegularPolygonAngle(7), 0.01f) << "FAILURE CALCULATING REGULAR HEPTAGON ANGLE";
    EXPECT_NEAR(108.0, ts.calculateRegularPolygonAngle(5), 0.01f) << "FAILURE CALCULATING REGULAR PENTAGON ANGLE";
}

TEST_F(TurtleSoupTest, TestPoligonSideFromAngle) {
    EXPECT_EQ(3, ts.calculatePolygonSidesFromAngle(60.0));
    EXPECT_EQ(7, ts.calculatePolygonSidesFromAngle(128.57));
    EXPECT_EQ(5, ts.calculatePolygonSidesFromAngle(108.0));
}

TEST_F(TurtleSoupTest, CalculateHeadingstoPointTest) {
    EXPECT_NEAR(0.0f, ts.calculateHeadingToPoint(0.0f, 0.0f, 0.0f, 0.0f, 1.0f), 0.001f);
    EXPECT_NEAR(90.0f, ts.calculateHeadingToPoint(0.0f, 0.0f, 0.0f, 1.0f, 0.0f), 0.001f);
    EXPECT_NEAR(359.0f, ts.calculateHeadingToPoint(1.0f, 4.0f, 5.0f, 4.0f, 6.0f), 0.001f);
}

TEST_F(TurtleSoupTest, CalculateHeadingsTest) {
    std::vector<double> xPoints;
    std::vector<double> yPoints;
    xPoints.push_back(0);
    xPoints.push_back(1);
    xPoints.push_back(1);
    yPoints.push_back(0);
    yPoints.push_back(1);
    yPoints.push_back(2);

    std::vector<double> result = ts.calculateHeadings(xPoints, yPoints);
    EXPECT_EQ(2, result.size());
    EXPECT_NEAR(45.0f, result[0], 0.001f);
    EXPECT_NEAR(315.0f, result[1], 0.001f);
}
}