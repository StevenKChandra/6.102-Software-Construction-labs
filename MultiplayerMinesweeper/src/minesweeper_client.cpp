#include "minesweeper_client.h"

#include <iostream>
#include <openssl/err.h>
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
    if (!SSL_set_fd(ssl, impl->socket)) throw std::runtime_error("SSL structure creatopm failed");

    SSL_set_tlsext_host_name(ssl, impl->server_ip);
    if (!SSL_set1_host(ssl, impl->server_ip)) throw std::runtime_error("Hostname check failed");

    if (SSL_connect(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("SSL connection to server failed");
    }
    char buffer[BUFFER_LEN];
    memset(buffer, 0, BUFFER_LEN);
    
    while (true) {
        int read_len = read(0, buffer, BUFFER_LEN);

        if (read_len == -1) throw std::runtime_error("Read from stdin failed");

        int write_len = SSL_write(ssl, buffer, read_len);

        if (write_len <= 0) {
            std::cout << "Server closed connection\n";
            break;
        }
        
        if (strcmp("disconnect\n", buffer) == 0) break;
    }

    std::cout << "Client exiting...\n";
    
    SSL_shutdown(ssl);
    SSL_free(ssl);
}

int main() {
    char *ip = "localhost";
    MinesweeperClient client = MinesweeperClient(PORT, ip);
    client.connect();
    return 0;
}