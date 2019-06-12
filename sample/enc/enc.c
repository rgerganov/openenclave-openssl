// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <openenclave/enclave.h>
#include <malloc.h>
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
        oe_abort();
    }

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Unable to bind");
        oe_abort();
    }

    if (listen(s, 1) < 0) {
        printf("Unable to listen");
        oe_abort();
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
        oe_abort();
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx, char *cwd)
{
    char cert_path[PATH_MAX];
    char key_path[PATH_MAX];

    sprintf(cert_path, "%s%s", cwd, "/cert.pem");
    sprintf(key_path, "%s%s", cwd, "/key.pem");


    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, cert_path, SSL_FILETYPE_PEM) <= 0) {
        oe_abort();
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, key_path, SSL_FILETYPE_PEM) <= 0) {
        oe_abort();
    }
}

void run_server(char *cwd)
{
    int sock;
    SSL_CTX *ctx = NULL;
    SSL *ssl = NULL;
    int client = 0;

    if (OE_OK != oe_load_module_host_socket_interface())
    {
        printf("Error loading host socket interface\n");
        oe_abort();      
    }
    if (OE_OK != oe_load_module_host_resolver())
    {
        printf("Error loading host resolver\n");
        oe_abort();
    }
    if (OE_OK != oe_load_module_host_file_system())
    {
        printf("Error loading host file system\n");
        oe_abort();
    }
    if (mount("/", "/", "oe_host_file_system", MS_RDONLY, NULL))
    {
        printf("Error mounting host file system\n");
        oe_abort();
    }

    init_openssl();
    if(!(ctx = create_context()))
    {
       printf("Error creating context\n");
       oe_abort();
    }

    configure_context(ctx, cwd);

    if(!(sock = create_socket(4433)))
    {
        oe_abort();
    }

   const char reply[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 23\nConnection: Close\n\nHello from an enclave!\n";
    /* Handle connections */
    while (1) {
        struct sockaddr_in addr;
        uint len = sizeof(addr);

        client = accept(sock, (struct sockaddr*)&addr, &len);
        printf("accepted\n");
        if (client < 0)
        {
            printf("Unable to accept");
            oe_abort();
        }

        if(!(ssl = SSL_new(ctx)))
        {
            oe_abort();
        }
        if(!SSL_set_fd(ssl, client))
        {
            oe_abort();
        }

        if (SSL_accept(ssl) <= 0)
        {
            oe_abort();
        }
        else 
        {
            char buffer[100000] = { 0 };
            size_t readbytes = 0;
            if(SSL_read_ex(ssl, buffer, 100000, &readbytes) <= 0)
            {
                oe_abort();
            }
            if(SSL_write(ssl, reply, (int)strlen(reply)) <= 0)
            {
                oe_abort();
            }
            break;
        }
    }
    if (client)
        close(client);
    if(ssl)
        SSL_free(ssl);
    if(sock)
        close(sock);
    if(ctx)
       SSL_CTX_free(ctx);
    cleanup_openssl();
}
