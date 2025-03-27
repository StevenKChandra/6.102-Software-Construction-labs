#ifndef BOARD_H
#define BOARD_H

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>

enum struct TILE_DISPLAY {
    UNTOUCHED,
    FLAGGED,
    DUG,
};

enum struct TILE_HIDDEN {
    BOMB,
    EMPTY,
};

/**
 * A mutable minesweeper board consisting tiles of specified length and width.
 * All tiles start untouched, some tiles have bomb hidden.
 */
class Board {
    /**
     * Abstraction function:
     *      -
     * 
     */
public:
    Board() = delete;
    
    virtual ~Board() {};    
    
    /**
     * Constructs a new board given the width, length, and bomb count.
     * The bombs are spread with pseudo-random function.
     * Accepts an optional seed parameter which determines
     * the position of the bombs in the board.
     * 
     * @param y_size the width of the board, must be a positive integer
     * @param x_size the length of the board, must be a positive integer
     * @param bomb_count number of bomb in the board, must be non negative integer
     *                   cannot be bigger than the size of the board
     * @param seed the seed to generate the board
     * @throw std::domain_error if length or width is not positive or bomb_count is negative
     *        std::runtime_error if bomb_count is bigger than the size of the board
     */
    Board(int y_size, int x_size, int bomb_count, uint64_t seed = (uint64_t) std::time(nullptr)) {};

    /**
     * Write the player's board representation in a pointer of char (buffer).
     * 
     * @return a unique pointer to the buffer.
     */
    virtual std::unique_ptr<char[]> print() = 0;

    /**
     * Digs the tile at position given by the x and y input.
     * 
     * @param x the x position of the target tile.
     * @param y the y position of the target tile.
     * @return false if dig succeed and dug a bomb, true otherwise
     */
    virtual bool dig(int y, int x) noexcept = 0;

    /**
     * Flags the tile at position given by the x and y input
     * 
     * @param x the x position of the target tile.
     * @param y the y position of the target tile.
     */
    virtual void flag(int y, int x) noexcept = 0;
    
    /**
     * Deflags the tile at position given by the x and y input
     * 
     * @param x the x position of the target tile.
     * @param y the y position of the target tile.
     */
    virtual void deflag(int y, int x) noexcept = 0;
};

#endif