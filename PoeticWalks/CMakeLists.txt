cmake_minimum_required(VERSION 3.28.0)

set(CXX_STANDARD C++20)
set(CXX_STANDARD_REQUIRED True)

project(PoeticWalks VERSION 1.0 LANGUAGES CXX)

set(
    SRC_FILES
    src/graph/concrete_edges_graph.tpp
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
    test/graph/concrete_edges_graph_test.cpp
)

set(CMAKE_BUILD_TYPE Debug)
add_executable(PoeticWalksTest "${TEST_FILES}" "${SRC_FILES}")

target_include_directories(
    PoeticWalksTest
    PRIVATE src/graph
)

target_link_libraries(PoeticWalksTest GTest::gtest_main)

gtest_discover_tests(PoeticWalksTest)