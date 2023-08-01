#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define COAP_DTLS_PORT 5684

/* CoAP-DTLS server */
int handle_coap_request(SSL *ssl, uint8_t *buffer, int len)
{
    /* CoAP message format: Ver(2) | T(2) | TKL(4) | Code(8) | Message ID(16) */
    if (len < 4) {
        return -1;
    }
    
    uint8_t ver_t_tkl = buffer[0];
    uint8_t code = buffer[1];
    
    printf("CoAP request: Code=0x%02x\n", code);
    
    /* Send CoAP response */
    uint8_t response[4] = {0x60, 0x45, 0x00, 0x01}; /* 2.05 Content */
    SSL_write(ssl, response, sizeof(response));
    
    return 0;
}

int main(void)
{
    int sock;
    SSL_CTX *ctx;
    struct sockaddr_in addr;
    
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    
    ctx = SSL_CTX_new(DTLS_server_method());
    SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM);
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(COAP_DTLS_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    
    printf("CoAP-DTLS server listening on port %d\n", COAP_DTLS_PORT);
    
    while (1) {
        uint8_t buffer[1024];
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        
        int bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
                            (struct sockaddr *)&client_addr, &len);
        
        if (bytes > 0) {
            SSL *ssl = SSL_new(ctx);
            SSL_set_fd(ssl, sock);
            SSL_set_accept_state(ssl);
            
            if (SSL_accept(ssl) > 0) {
                handle_coap_request(ssl, buffer, bytes);
            }
            
            SSL_free(ssl);
        }
    }
    
    close(sock);
    SSL_CTX_free(ctx);
    return 0;
}

