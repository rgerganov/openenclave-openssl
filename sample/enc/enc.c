// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <openenclave/enclave.h>
#include <malloc.h>
//#include <raise.h>
//#include <rsa.h>
//#include <sha.h>
#include <syscall.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/uio.h>
#include <unistd.h>
#include "openssl_server_t.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int create_socket(int port)
{
    (void)port;
    int s;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(4433);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
	printf("Unable to create socket");
	exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
	printf("Unable to bind");
	exit(EXIT_FAILURE);
    }

    if (listen(s, 1) < 0) {
	printf("Unable to listen");
	exit(EXIT_FAILURE);
    }

    return s;
}

void init_openssl()
{ 
    SSL_load_error_strings();	
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl()
{
    EVP_cleanup();
}

SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
	printf("Unable to create SSL context");
	exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
	exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0 ) {
	exit(EXIT_FAILURE);
    }
}

void test()
{
    oe_enable_feature(OE_FEATURE_HOST_FILES);
    oe_enable_feature(OE_FEATURE_HOST_SOCKETS);
    oe_enable_feature(OE_FEATURE_HOST_RESOLVER);
    oe_enable_feature(OE_FEATURE_POLLING);
    mount(".", "/", "hostfs", MS_RDONLY, NULL);

    int sock;
    SSL_CTX *ctx;

    init_openssl();
    ctx = create_context();

    configure_context(ctx);

    sock = create_socket(4433);
    

    /* Handle connections */
    while(1) {
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        SSL *ssl;
        const char reply[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 23\nConnection: Close\n\nHello from an enclave!\n";

        int client = accept(sock, (struct sockaddr*)&addr, &len);
        printf("accepted\n");
	if (client < 0) {
            printf("Unable to accept");
            exit(EXIT_FAILURE);
        }

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        if (SSL_accept(ssl) <= 0) {
        }
        else {
            char buffer[100000] = {0};
            size_t readbytes = 0;
            SSL_read_ex(ssl, buffer, 100000, &readbytes);
            SSL_write(ssl, reply, (int)strlen(reply));
        }

        SSL_free(ssl);
        close(client);
    }

    close(sock);
    SSL_CTX_free(ctx);
    cleanup_openssl();
}
