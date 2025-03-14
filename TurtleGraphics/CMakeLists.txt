cmake_minimum_required(VERSION 3.28.0)

set(CXX_STANDARD C++20)
set(CXX_STANDARD_REQUIRED True)

project(TurtleGraphics VERSION 1.0 LANGUAGES CXX)

set(
    SRC_FILES 
    src/action.cpp
    src/drawable_turtle.cpp
    src/line_segment.cpp
    src/point.cpp
    src/turtle_soup.cpp
    src/util.cpp
)

set(CMAKE_BUILD_TYPE Release) 
add_executable(TurtleGraphics src/main.cpp "${SRC_FILES}")

target_include_directories(
    TurtleGraphics
    PUBLIC src
)

# Find Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

enable_testing()

set(
    TEST_FILES
    test/turtle_soup_test.cpp
)

set(CMAKE_BUILD_TYPE Debug)
add_executable(TurtleGraphicsTest "${TEST_FILES}" "${SRC_FILES}")

target_include_directories(
    TurtleGraphicsTest
    PRIVATE test
    PRIVATE src
)
target_link_libraries(TurtleGraphicsTest GTest::gtest_main)

gtest_discover_tests(TurtleGraphicsTest)