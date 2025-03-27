#include "minesweeper_server.h"

#include <iostream>
#include <cstring>
#include <functional>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFFER_LEN 1024

struct MinesweeperServer::Private {
    int port;
    int server_socket;
    bool running;
    SSL_CTX *ctx;

    std::unordered_map<int, std::thread> client_threads;
    std::mutex client_mutex;

    Private(int port);
    ~Private();

    void accept_clients(Private *impl);
    void handle_client(Private *impl, int client_socket);
    void disconnect_client(Private *impl, int client_socket);
};

MinesweeperServer::Private::Private(int port):
port{port}, server_socket{-1}, running{false}, ctx{nullptr}, client_threads{}, client_mutex{} {}

MinesweeperServer::MinesweeperServer(int port):
impl(new Private{port}) {}
                                                                                        
void MinesweeperServer::start() {
    impl->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (impl->server_socket < 0) throw std::runtime_error("socket creation failed");

    sockaddr_in address{};
    address.sin_family = AF_INET6;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if(bind(impl->server_socket, (sockaddr*) &address, sizeof(address)) < 0) {
        close(impl->server_socket);
        throw std::runtime_error("bind failed");
    }
    if (listen(impl->server_socket, 5) < 0) throw std::runtime_error("listen failed");
    
    impl->ctx = SSL_CTX_new(TLS_server_method());
    if (impl->ctx == nullptr) throw std::runtime_error("Unable to create SSL context");

    if (SSL_CTX_use_certificate_chain_file(impl->ctx, "cert.pem") <= 0) {
        throw std::runtime_error("Unable use certificate file");
    }
    if (SSL_CTX_use_PrivateKey_file(impl->ctx, "key.pem", SSL_FILETYPE_PEM) <= 0) {
        throw std::runtime_error("Unable use certificate file");
    }
    
    std::thread(&Private::accept_clients, impl, impl).detach();
}

void MinesweeperServer::Private::accept_clients(Private* impl) {
    while (impl->running) {
        sockaddr_in clientAddress;
        socklen_t clilen = sizeof(clientAddress);

        int client_socket = accept(
            impl->server_socket,
            (struct sockaddr*) &clientAddress,
            &clilen
        );

        if (client_socket < 0) {
            if (!impl->running) break;
            std::cerr << "Accept failed\n";
            continue;
        }

        std::cout << "New client connected!\n";
        impl->client_mutex.lock();

        if (!impl->running) {
            impl->client_mutex.unlock();
            return;
        }

        impl->client_threads[client_socket] = std::thread(
            &Private::handle_client,
            this,
            this,
            client_socket
        );
        impl->client_mutex.unlock();
    }
}

void MinesweeperServer::Private::handle_client(Private *impl, int client_socket) {
    SSL *ssl = SSL_new(impl->ctx);

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

    std::cout << "Client SSL connection accepted\n";

    const char welcomeMsg[] = "Welcome to the Minesweeper Server!\n";
    SSL_write(ssl, welcomeMsg, strlen(welcomeMsg));

    char buffer[BUFFER_LEN] {};
    int read_len{0};
    while (impl->running) {
        read_len = SSL_read(ssl, buffer, BUFFER_LEN);
        
        if (read_len == 0) {
            std::cout << "Client closed connection\n";
            break;
        }

        if (read_len < 0) {
            std::cerr << "SSL_read retuned " << read_len << '\n';
            break;
        }

        buffer[BUFFER_LEN - 1] = '\0';
        
        if (strcmp(buffer, "kill\n") == 0) {
            std::cout << "Received kill connection command\n";
            break;
        }
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_socket);
    
    std::cout << "Connection with client closed\n";
}

void MinesweeperServer::stop() {
    impl -> running = false;
    if (impl->server_socket != -1) {
        close(impl->server_socket);
        impl->server_socket = -1;
    }

    impl->client_mutex.lock();
    for (std::pair<const int, std::thread>& client: impl->client_threads) {
        if (!client.second.joinable()) continue;
        close(client.first);
        client.second.join();
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
    char buffer[1024];
    read(0, buffer, 1024);
    return 0;
}