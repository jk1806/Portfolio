#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/dtls1.h>

#define PORT 5684
#define CERT_FILE "server.crt"
#define KEY_FILE "server.key"

SSL_CTX *create_dtls_context(void)
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    
    method = DTLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create DTLS context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    return ctx;
}

void configure_dtls_context(SSL_CTX *ctx)
{
    if (SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    int sock;
    SSL_CTX *ctx;
    struct sockaddr_in addr;
    
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    
    ctx = create_dtls_context();
    configure_dtls_context(ctx);
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }
    
    printf("DTLS server listening on port %d\n", PORT);
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        SSL *ssl;
        char buf[1024];
        const char reply[] = "Hello from DTLS server\n";
        
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);
        SSL_set_accept_state(ssl);
        
        int ret = SSL_accept(ssl);
        if (ret <= 0) {
            ERR_print_errors_fp(stderr);
        } else {
            int bytes = SSL_read(ssl, buf, sizeof(buf) - 1);
            if (bytes > 0) {
                buf[bytes] = '\0';
                printf("Received: %s", buf);
                SSL_write(ssl, reply, strlen(reply));
            }
        }
        
        SSL_free(ssl);
    }
    
    close(sock);
    SSL_CTX_free(ctx);
    return 0;
}

