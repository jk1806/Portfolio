#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* QUIC protocol demo - simplified implementation */
#define QUIC_PORT 443

typedef struct {
    uint8_t flags;
    uint8_t version[4];
    uint8_t dcid_len;
    uint8_t dcid[20];
    uint8_t scid_len;
    uint8_t scid[20];
} quic_header_t;

int quic_send_packet(int sock, const uint8_t *data, size_t len)
{
    quic_header_t header;
    
    /* Build QUIC header */
    header.flags = 0xC0; /* Long header, version negotiation */
    memcpy(header.version, "\x00\x00\x00\x01", 4); /* QUIC version 1 */
    header.dcid_len = 8;
    memset(header.dcid, 0, 20);
    header.scid_len = 8;
    memset(header.scid, 0, 20);
    
    /* Send header */
    send(sock, &header, sizeof(header), 0);
    
    /* Send data */
    send(sock, data, len, 0);
    
    return 0;
}

int main(void)
{
    int sock;
    struct sockaddr_in addr;
    const char *msg = "Hello from QUIC\n";
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(QUIC_PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    
    printf("QUIC demo - sending packet\n");
    quic_send_packet(sock, (uint8_t *)msg, strlen(msg));
    
    close(sock);
    return 0;
}

