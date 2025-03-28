#ifndef UTIL_H
#define UTIL_H

#include <openssl/ssl.h>

namespace Util {
    int create_server_socket(int port);

    int create_client_socket(int port, char *server_ip);
    
    SSL_CTX* create_context(bool is_server);

    void configure_server_context(SSL_CTX *ctx);

    void configure_client_context(SSL_CTX *ctx);

    void set_non_blocking(int fd);
}

#endif