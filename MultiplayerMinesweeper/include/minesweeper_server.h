#ifndef MINESWEEPER_SERVER_H
#define MINESWEEPER_SERVER_H

#include <mutex>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdexcept>
#include <thread>

#include "board.h"
#include "board_implementation.h"

#define PORT 9023

class MinesweeperServer {
private:
    struct Private;
    Private *impl;
public:
    MinesweeperServer() = delete;

    MinesweeperServer(const MinesweeperServer& that) = delete;

    MinesweeperServer& operator=(const MinesweeperServer& that) = delete;

    MinesweeperServer(MinesweeperServer&& that) = delete;

    MinesweeperServer& operator=(MinesweeperServer&& that) = delete;

    ~MinesweeperServer();

    /**
     * Mineswepeer server that listens for connections on port.
     * 
     * @param port port number, requires 0 <= port <= 65535
     * @throws std::runtime_error if error occurs when opening the server socket
     */
    MinesweeperServer(int port);

    /**
     * Start the server, listening for client connection and handling them.
     * @throws std::runtime_error if the main server socket is broken
     */
    void start();
    
    /**
     * 
     */
    void stop();
};

#endif