cmake_minimum_required(VERSION 3.28.0)

set(CXX_STANDARD C++ 17)
set(CXX_STANDARD_REQUIRED true)

project(MultiplayerMinesweeper VERSION 1.0 LANGUAGES CXX)

set(
    SRC_FILES
    src/board/board.cpp
)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip 
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)
enable_testing()

set (
    TEST_FILES
    test/board/board_test.cpp
)

set(CMAKE_BUILD_TYPE Debug)
add_executable(MultiplayerMinesweeperTest "${TEST_FILES}" "${SRC_FILES}")

target_include_directories(
    MultiplayerMinesweeperTest
    PRIVATE src/board
)

target_link_libraries(MultiplayerMinesweeperTest GTest::gtest_main)

gtest_discover_tests(MultiplayerMinesweeperTest)