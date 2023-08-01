#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 8443
#define CERT_FILE "server.crt"
#define KEY_FILE "server.key"
#define CA_CERT "ca.crt"

SSL_CTX *create_mtls_context(void)
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    
    method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    return ctx;
}

void configure_mtls_context(SSL_CTX *ctx)
{
    /* Load server certificate */
    if (SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    /* Load server private key */
    if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    /* Load CA certificate for client verification */
    if (SSL_CTX_load_verify_locations(ctx, CA_CERT, NULL) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    /* Require client certificate */
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
}

int main(void)
{
    int sock, client;
    SSL_CTX *ctx;
    struct sockaddr_in addr;
    
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    
    ctx = create_mtls_context();
    configure_mtls_context(ctx);
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    listen(sock, 5);
    
    printf("mTLS server listening on port %d\n", PORT);
    
    while (1) {
        client = accept(sock, NULL, NULL);
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);
        
        if (SSL_accept(ssl) > 0) {
            X509 *client_cert = SSL_get_peer_certificate(ssl);
            if (client_cert) {
                printf("Client certificate verified\n");
                X509_free(client_cert);
            }
            
            const char *reply = "Hello from mTLS server\n";
            SSL_write(ssl, reply, strlen(reply));
        }
        
        SSL_free(ssl);
        close(client);
    }
    
    close(sock);
    SSL_CTX_free(ctx);
    return 0;
}

