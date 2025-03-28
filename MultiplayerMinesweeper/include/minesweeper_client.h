#ifndef MINESWEEPER_CLIENT_H
#define MINESWEEPER_CLIENT_H

class MinesweeperClient {
private:
    struct Private;
    Private *impl;
public:
    MinesweeperClient() = delete;

    MinesweeperClient(const MinesweeperClient& that) = delete;

    MinesweeperClient& operator=(MinesweeperClient& that) = delete;

    MinesweeperClient(MinesweeperClient&& that) = delete;

    MinesweeperClient& operator=(MinesweeperClient&& that) = delete;

    MinesweeperClient(int port, char* server_ip);
    
    ~MinesweeperClient();

    void connect();

    void disconnect();
};

#endif