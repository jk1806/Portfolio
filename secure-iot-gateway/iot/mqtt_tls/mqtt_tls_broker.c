#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MQTT_TLS_PORT 8883
#define CERT_FILE "server.crt"
#define KEY_FILE "server.key"

/* Simplified MQTT-TLS broker */
int handle_mqtt_connect(SSL *ssl)
{
    uint8_t buffer[1024];
    int bytes = SSL_read(ssl, buffer, sizeof(buffer));
    
    if (bytes > 0 && buffer[0] == 0x10) { /* CONNECT packet */
        printf("MQTT CONNECT received\n");
        
        /* Send CONNACK */
        uint8_t connack[] = {0x20, 0x02, 0x00, 0x00};
        SSL_write(ssl, connack, sizeof(connack));
        return 0;
    }
    
    return -1;
}

int main(void)
{
    int sock, client;
    SSL_CTX *ctx;
    struct sockaddr_in addr;
    
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    
    ctx = SSL_CTX_new(TLS_server_method());
    SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM);
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(MQTT_TLS_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    listen(sock, 5);
    
    printf("MQTT-TLS broker listening on port %d\n", MQTT_TLS_PORT);
    
    while (1) {
        client = accept(sock, NULL, NULL);
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);
        
        if (SSL_accept(ssl) > 0) {
            handle_mqtt_connect(ssl);
        }
        
        SSL_free(ssl);
        close(client);
    }
    
    close(sock);
    SSL_CTX_free(ctx);
    return 0;
}

