#include <gtest/gtest.h>

#include <stdexcept>
#include <memory>
#include <iostream>

#include "board.h"

namespace {

class BoardTest: public ::testing::Test {
protected:
    void SetUp(void) {
        front = new char[10 * 11] { "UUUUUUUUUU\nUUUUUUUUUU\nUUUUUUUUUU\nUUUUUUUUUU\nUUUUUUUUUU\nUUUUUUUUUU\nUUUUUUUUUU\nUUUUUUUUUU\nUUUUUUUUUU\nUUUUUUUUUU" };
        back = new char[10 * 11] { "EEEEEEEEEE\nEBEEEEEEEE\nEEEEEEEEEE\nEEEEEEBEEB\nEEEBBEEEEB\nEEEEEEEEEE\nBEEEEEBEEE\nEEEEEEEEEE\nEEEEEEEEEB\nEEBEEEEEEE" };
        boundaries  = new char[10 * 11] { "1110000000\n1010000000\n1110011111\n0012220121\n0011121121\n1112221111\n0100010100\n1100011111\n0111000010\n0101000011" };
    }
    void TearDown(void) {
        delete front;
        delete back;
        delete boundaries;
    }
    void check_board_state() {
        EXPECT_STREQ(front, output["front"].get()) << "Expected correct board's frontside";
        EXPECT_STREQ(back, output["back"].get()) << "Expected correct board's backside";
        EXPECT_STREQ(boundaries, output["boundaries"].get()) << "Expected correct board's bomb boundaries number";    
    }
    Board board = Board(10, 10, 10, 0);
    std::unordered_map<std::string, std::unique_ptr<char []>> output;
    char *front;
    char *back;
    char *boundaries;
};

TEST_F(BoardTest, ConstructorTest) {
    ASSERT_NO_THROW(Board(10, 10, 10, 0)) << "Custructor must succeed if the parameters are valid";
    EXPECT_THROW(Board(10, 0, 10, 0), std::domain_error) << "Expected constructor to throw error when one of the size is non positive";
    EXPECT_THROW(Board(8, 15, 200, 0), std::runtime_error) << "Expected constructor to throw error when bomb count is bigger than board size";
}

TEST_F(BoardTest, PrintTest) {
    
}

TEST_F(BoardTest, DigTest) {
    /**
     * Testing strategy
     * partition on tile condition:
     *      - out of bound
     *      - flagged
     *      - not flagged, no bomb
     *      - not flagged, with bomb
     *      - dug
     */

    // out of bound
    EXPECT_TRUE(board.dig(-1, 0)) << "Expected to return true when tile is out of bound";
    output = board.print_debug();
    check_board_state();

    // unflagged, no bomb
    EXPECT_TRUE(board.dig(4, 1)) << "Expected to return true when digging unflagged tile a bomb";
    output = board.print_debug();
    delete front;
    front = new char[10 * 11] { "UUUUUUUUUU\nUUUUUUUUUU\nDDDUUUUUUU\nDDDUUUUUUU\nDDDUUUUUUU\nDDDUUUUUUU\nUUUUUUUUUU\nUUUUUUUUUU\nUUUUUUUUUU\nUUUUUUUUUU" };
    check_board_state();

    // dug
    EXPECT_TRUE(board.dig(4, 1)) << "Expected to return true when digging a dug tile";
    output = board.print_debug();
    check_board_state();

    // flagged
    board.flag(1, 1);
    EXPECT_TRUE(board.dig(1, 1)) << "Expected to return true when digging a flagged tile";
    output = board.print_debug();
    front[1 * 11 + 1] = 'F';
    check_board_state();

    // unflagged, with bomb
    EXPECT_FALSE(board.dig(3, 9)) << "Expected to return false when digging a bomb";
    output = board.print_debug();
    front[3 * 11 + 9] = 'D';
    back[2 * 11 + 8] = 'E';
    boundaries[2 * 11 + 8]--;
    boundaries[2 * 11 + 9]--;
    boundaries[3 * 11 + 8]--;
    boundaries[4 * 11 + 8]--;
    boundaries[4 * 11 + 9]--;
    check_board_state();
}

TEST_F(BoardTest, FlagTest) {
    /**
     * Testing strategy:
     * partition on tiles:
     *      - coordiate out of bound
     *      - not flagged, without boomb
     *      - flagged, without bomb
     *      - not flagged, with bomb
     *      - flagged with bound
     *      - dug
     */

    // out of bound
    board.flag(0, 10);
    output = board.print_debug();
    check_board_state();

    // not flagged, without bomb
    board.flag(7, 7);
    output = board.print_debug();
    front[7 * 11 + 7] = 'F';
    check_board_state();
    
    // flagged, without bomb
    board.flag(7, 7);
    output = board.print_debug();
    check_board_state();

    // unflagged, with bomb
    board.flag(6, 0);
    output = board.print_debug();
    front[6 * 11 + 0] = 'F';
    check_board_state();

    // flagged, with bomb
    board.flag(6, 0);
    output = board.print_debug();
    check_board_state();
}

TEST_F(BoardTest, DeflagTest) {
    /**
     * Testing strategy:
     * partition on tiles:
     *      - coordiate out of bound
     *      - not flagged, without boomb
     *      - flagged, without bomb
     *      - not flagged, with bomb
     *      - flagged with bound
     *      - dug
     */

    // out of bound
    board.deflag(-1, 100);
    output = board.print_debug();
    check_board_state();

    // not flagged, without bomb
    board.deflag(7, 7);
    output = board.print_debug();
    check_board_state();
    
    // flagged, without bomb
    board.flag(8, 8);
    board.deflag(8, 8);
    output = board.print_debug();
    check_board_state();

    // unflagged, with bomb
    board.deflag(8, 9);
    output = board.print_debug();
    check_board_state();

    // flagged, with bomb
    board.flag(8, 9);
    board.deflag(8, 9);
    output = board.print_debug();
    check_board_state();
}
}