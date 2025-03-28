#include "util.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

int Util::create_server_socket(int port) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) throw std::runtime_error("socket creation failed");

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(bind(socket_fd, (sockaddr*) &address, sizeof(address)) < 0) {
        close(socket_fd);
        throw std::runtime_error("bind failed");
    }

    if (listen(socket_fd, 5) < 0) throw std::runtime_error("listen failed");
    
    return socket_fd;
}

int Util::create_client_socket(int port, char *server_ip) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) throw std::runtime_error("socket creation failed");

    sockaddr_in address{};
    address.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &address.sin_addr.s_addr);
    address.sin_port = htons(port);

    if (connect(socket_fd, (struct sockaddr*) &address, sizeof(address)) != 0){
        close(socket_fd);
        throw std::runtime_error("TCP connection to server failed");
    }
    return socket_fd;
}

SSL_CTX* Util::create_context(bool is_server) {
    const SSL_METHOD* method;
    SSL_CTX* ctx;

    if (is_server) {
        method = TLS_server_method();
    }
    else {
        method = TLS_client_method();
    }

    ctx = SSL_CTX_new(method);
    if (ctx == nullptr) {
        throw std::runtime_error("Unable to create SSL context");
    }

    return ctx;
}

void Util::configure_server_context(SSL_CTX* ctx) {
    if (SSL_CTX_use_certificate_chain_file(ctx, "cert.pem") <= 0) {
        throw std::runtime_error("Unable to install public key certificate");
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0) {
        throw std::runtime_error("Unable to install private key");
    }
}

void Util::configure_client_context(SSL_CTX* ctx) {
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    /**
     * In a real application you would probably just use the default system 
     *  certificate trust store and call SSL_CTX_set_default_verify_paths(ctx);
     * 
     * In this demo though we are using a self-signed certificate, so the client
     *  must trust it directly.
     */
    if (!SSL_CTX_load_verify_locations(ctx, "cert.pem", NULL)) {
        throw std::runtime_error("Unable to verify public key certificate");
    }
}

void Util::set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) throw std::runtime_error("Cannot get socket status flag");
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}