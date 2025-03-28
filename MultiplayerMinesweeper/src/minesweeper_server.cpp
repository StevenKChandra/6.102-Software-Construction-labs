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

        Util::set_non_blocking(client_socket);

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

    while (true) {
        int return_code = SSL_accept(ssl);
        if (return_code == 1) {
            std::cout << "Client SSL connection accepted\n";
            break;
        }
        int error_code = SSL_get_error(ssl, return_code);
        if (error_code == SSL_ERROR_WANT_READ || error_code == SSL_ERROR_WANT_WRITE) {
            struct pollfd pfd{};
            pfd.fd = client_socket;
            pfd.events = (error_code == SSL_ERROR_WANT_READ) ? POLLIN : POLLOUT;
            poll(&pfd, 1, -1);
        }
        else {
            ERR_print_errors_fp(stderr);
            std::cerr << "SSL connection to client failed\n";
            SSL_free(ssl);
            close(client_socket);
            return;
        }
    }
    // const char welcomeMsg[] = "Welcome to the Minesweeper Server!\n";
    // SSL_write(ssl, welcomeMsg, strlen(welcomeMsg));

    char buffer[BUFFER_LEN] {};
    int read_len{0};

    struct pollfd pfd{};
    pfd.fd = client_socket;
    pfd.events = POLLIN;

    while (running) {

        poll(&pfd, 1, 1000);

        if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            SSL_free(ssl);
            if ((pfd.revents & POLLNVAL)) return;
            std::cerr << "Socket closed or hanged up\n";
            close(socket);
            return;
        }

        if (pfd.revents && POLLIN) {
            read_len = SSL_read(ssl, buffer, BUFFER_LEN);
            
            if (read_len < 0) {
                int error_code = SSL_get_error(ssl, read_len);
                if (error_code == SSL_ERROR_WANT_READ) continue;
                std::cout << "SSL_read retuned " << read_len << '\n';
                SSL_free(ssl);
                close(client_socket);
                return;
            }
            
            if (read_len == 0) {
                std::cout << "Client closed connection\n";
                break;
            }
    
            buffer[read_len] = '\0';
            
            if (strcmp(buffer, "disconnect\n") == 0) {
                std::cout << "Received kill connection command\n";
                break;
            }
    
            write(1, buffer, read_len);
        }
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_socket);
    
    std::cout << "Connection with client closed\n";
    return;
}

void MinesweeperServer::stop() {
    impl -> running = false;
    if (impl->socket != -1) {
        close(impl->socket);
        impl->socket = -1;
    }

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