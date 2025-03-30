#include "minesweeper_client.h"

#include <iostream>
#include <openssl/err.h>
#include <poll.h>
#include <unistd.h>

#include "util.h"

#define PORT 9023
#define BUFFER_LEN 1024

struct MinesweeperClient::Private {
    bool running;
    int port;
    char *server_ip;
    int socket;
    SSL_CTX* ctx;

    Private(int port, char *server_ip);
    ~Private();
};

MinesweeperClient::Private::Private(int port, char *server_ip):
running{false}, port{port}, server_ip{server_ip}, socket{-1}, ctx{nullptr} {}

MinesweeperClient::MinesweeperClient(int port, char *server_ip):
impl{new Private{port, server_ip}} {}

MinesweeperClient::Private::~Private() {
    close(socket);
}

MinesweeperClient::~MinesweeperClient() {
    delete impl;
}

void MinesweeperClient::connect() {
    impl->socket = Util::create_client_socket(impl->port, impl->server_ip);
    impl->ctx = Util::create_context(false);
    Util::configure_client_context(impl->ctx);

    SSL *ssl = SSL_new(impl->ctx);
    if (!SSL_set_fd(ssl, impl->socket)) throw std::runtime_error("Binding fd to ssl failed");

    SSL_set_tlsext_host_name(ssl, impl->server_ip);
    if (!SSL_set1_host(ssl, impl->server_ip)) throw std::runtime_error("Hostname check failed");

    if (SSL_connect(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("SSL connection to server failed");
    }
    char buffer[BUFFER_LEN];
    memset(buffer, 0, BUFFER_LEN);
    
    while (true) {
        struct pollfd pfd[2];
        struct pollfd *stdin_pfd = pfd;
        struct pollfd *socket_pfd = (pfd + 1);
        stdin_pfd->fd = 0;
        stdin_pfd->events = POLLIN;
        socket_pfd->fd = impl->socket;
        socket_pfd->events = 0;

        if (poll(pfd, 2, 1000) == 0) {
            write(impl->socket, buffer, 0);
            continue;
        }

        // other end of stream socket is closed
        if (socket_pfd->revents & (POLLERR | POLLHUP)) {
            std::cout << "Server closed connection!\n";
            break;
        }

        int read_len = read(0, buffer, BUFFER_LEN);

        int write_len = SSL_write(ssl, buffer, read_len);

        if (write_len <= 0) {
            std::cout << "Send message failed\n";
            break;
        }
        
        if (strcmp("disconnect\n", buffer) == 0) {
            std::cout << "Disconnecting...\n";
            break;
        }
    }
}

int main() {
    char *ip = "localhost";
    MinesweeperClient client = MinesweeperClient(PORT, ip);
    client.connect();
    return 0;
}