#include "minesweeper_client.h"

#include <iostream>
#include <openssl/err.h>
#include <sys/epoll.h>
#include <sys/socket.h>
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
    
    #define MAX_EVENTS 2
    struct epoll_event ev, events[MAX_EVENTS];
    int epoll_fd, nfds;

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        throw std::runtime_error("Epoll create failed");
    }

    ev.data.fd = 0;
    ev.events = EPOLLIN;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &ev) == -1) {
        throw std::runtime_error("epoll ctl on stdin failed");
    }
    ev.data.fd = impl->socket;
    ev.events = EPOLLIN;
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, impl->socket, &ev) == -1) {
        throw std::runtime_error("epoll ctl on tcp socket failed");
    }

    Util::set_non_blocking(0);

    while (true) {
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == impl->socket && (events[i].events & (EPOLLHUP|EPOLLERR))) {
                std::cout<<"Server closed connection\n";
                return;
            }

            int read_len = read(0, buffer, BUFFER_LEN);

            if (read_len == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) continue;

            int write_len = SSL_write(ssl, buffer, read_len);

            if (write_len <= 0) {
                std::cout << "Send message failed\n";
                return;
            }
            
            if (strcmp("disconnect\n", buffer) == 0) {
                std::cout << "Disconnecting...\n";
                return;
            }
        }
    }
}

int main() {
    char *ip = "localhost";
    MinesweeperClient client = MinesweeperClient(PORT, ip);
    client.connect();
    return 0;
}