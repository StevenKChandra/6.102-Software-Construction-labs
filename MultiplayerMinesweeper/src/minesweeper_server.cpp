#include "minesweeper_server.h"

#include <cstring>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <openssl/err.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>

#include "util.h"

#define PORT 9023
#define BUFFER_LEN 1024

struct MinesweeperServer::Private {
    bool running;
    int port;
    int socket;
    SSL_CTX *ctx;

    std::unordered_map<int, std::thread*> client_threads;
    std::mutex client_mutex;

    Private(int port);
    ~Private();

    void accept_clients();
    void handle_client(int client_socket);
};

MinesweeperServer::Private::Private(int port):
running{false}, port{port}, socket{-1}, ctx{nullptr}, client_threads{}, client_mutex{} {}

MinesweeperServer::MinesweeperServer(int port):
impl(new Private{port}) {}
                                                                                        
void MinesweeperServer::start() {
    impl->socket = Util::create_server_socket(impl->port);
    impl->ctx = Util::create_context(true);
    Util::configure_server_context(impl->ctx);
    impl->running = true;
    std::thread(&Private::accept_clients, impl).detach();
}

void MinesweeperServer::Private::accept_clients() {
    sockaddr_in clientAddress;
    socklen_t clilen = sizeof(clientAddress);

    while (running) {
        int client_socket = accept(
            socket,
            (struct sockaddr*) &clientAddress,
            &clilen
        );

        if (client_socket < 0) {
            if (!running) break;
            std::cerr << "Accept failed\n";
            continue;
        }

        // Util::set_non_blocking(client_socket);

        std::cout << "New client connected!\n";
        client_mutex.lock();
        if (client_threads.find(client_socket) != client_threads.end()) {
            if (client_threads[client_socket]->joinable()) {
                client_threads[client_socket]->join();
            }
            delete client_threads[client_socket];  
        }
        client_threads[client_socket] = new std::thread(
            &Private::handle_client,
            this,
            client_socket
        );
        client_mutex.unlock();
    }
}

void MinesweeperServer::Private::handle_client(int client_socket) {
    SSL *ssl = SSL_new(ctx);

    if (!SSL_set_fd(ssl, client_socket)) {
        std::cerr<< "SSL setup failed\n";
        close(client_socket);
        return;
    }

    if (SSL_accept(ssl) <= 0) {
        std::cerr<< "SSL connection to client failed\n";
        SSL_free(ssl);
        close(client_socket);
        return;
    }

    // const char welcomeMsg[] = "Welcome to the Minesweeper Server!\n";
    // SSL_write(ssl, welcomeMsg, strlen(welcomeMsg));

    while (running) {
        struct pollfd pfd{};
        pfd.fd = client_socket;
        pfd.events = POLLIN;

        // no events or error
        if (poll(&pfd, 1, 1000) == 0) continue;
        
        // the server side stream socket is closed
        if (pfd.revents & POLLNVAL) {
            SSL_free(ssl);
            break;
        }
        
        // other end of stream socket is closed
        if (pfd.revents & (POLLERR | POLLHUP)) {
            std::cout << "Client closed connection!\n";
            SSL_free(ssl);
            close(client_socket);
            break;
        }

        // there is data to read
        char buffer[BUFFER_LEN] {};
        int read_len{0};
        read_len = SSL_read(ssl, buffer, BUFFER_LEN);
        buffer[read_len] = '\0';
        write(1, buffer, read_len);
    }
    return;
}

void MinesweeperServer::stop() {
    impl -> running = false;

    impl->client_mutex.lock();
    for (std::pair<const int, std::thread*>& client: impl->client_threads) {
        if (!client.second->joinable()) continue;
        close(client.first);
        client.second->join();
        delete client.second;
    }
    impl->client_threads.clear();
    impl->client_mutex.unlock();

    
    SSL_CTX_free(impl->ctx);
    
    if (impl->socket != -1) {
        close(impl->socket);
        impl->socket = -1;
    }
    
    std::cout << "Server exiting...\n";
}

MinesweeperServer::Private::~Private() { }

MinesweeperServer::~MinesweeperServer() {
    stop();
    delete impl;
}

int main() {
    MinesweeperServer server{PORT};
    server.start();
    char buffer[1024];
    read(0, buffer, 1024);
    return 0;
}