/**
 * TFTP Boot Implementation
 * Author: jk1806
 * Created: 2024-05-15
 * 
 * Trivial File Transfer Protocol for network booting
 * Supports kernel and initramfs loading over network
 */

#include <common.h>
#include <net.h>
#include <tftp.h>
#include <image.h>

#define TFTP_VERSION "1.0"
#define TFTP_BLOCK_SIZE 512
#define TFTP_MAX_RETRIES 5
#define TFTP_TIMEOUT_MS 5000

struct tftp_context {
    ulong server_ip;
    char *filename;
    ulong load_addr;
    size_t file_size;
    u16 block_num;
    u16 last_block;
    int retries;
    enum tftp_state {
        TFTP_STATE_IDLE,
        TFTP_STATE_RRQ,
        TFTP_STATE_DATA,
        TFTP_STATE_ACK,
        TFTP_STATE_ERROR
    } state;
};

static struct tftp_context tftp_ctx;

/**
 * Send TFTP Read Request (RRQ)
 */
static int tftp_send_rrq(const char *filename, const char *mode)
{
    uchar *pkt;
    int len;
    int pkt_len;
    
    pkt = net_tx_packet + net_eth_hdr_size() + IP_UDP_HDR_SIZE;
    p = pkt;
    
    // Opcode: RRQ (1)
    *p++ = 0;
    *p++ = 1;
    
    // Filename
    len = strlen(filename);
    memcpy(p, filename, len);
    p += len;
    *p++ = 0;
    
    // Mode
    len = strlen(mode);
    memcpy(p, mode, len);
    p += len;
    *p++ = 0;
    
    pkt_len = p - pkt;
    
    // Send packet
    net_send_udp_packet(net_server_ip, TFTP_SERVER_PORT, 
                        TFTP_CLIENT_PORT, pkt_len);
    
    tftp_ctx.state = TFTP_STATE_RRQ;
    tftp_ctx.retries = 0;
    
    printf("TFTP: Sent RRQ for '%s'\n", filename);
    
    return 0;
}

/**
 * Handle TFTP Data packet
 */
static int tftp_handle_data(uchar *pkt, int len)
{
    u16 opcode, block_num;
    u16 data_len;
    uchar *data;
    
    opcode = (pkt[0] << 8) | pkt[1];
    if (opcode != 3) {  // DATA opcode
        return -1;
    }
    
    block_num = (pkt[2] << 8) | pkt[3];
    data = pkt + 4;
    data_len = len - 4;
    
    // Check if this is the expected block
    if (block_num != tftp_ctx.block_num + 1) {
        printf("TFTP: Unexpected block number %d (expected %d)\n",
               block_num, tftp_ctx.block_num + 1);
        return -1;
    }
    
    // Copy data to load address
    memcpy((void *)(tftp_ctx.load_addr + (block_num - 1) * TFTP_BLOCK_SIZE),
           data, data_len);
    
    tftp_ctx.block_num = block_num;
    
    // Check if this is the last block
    if (data_len < TFTP_BLOCK_SIZE) {
        tftp_ctx.last_block = block_num;
        tftp_ctx.file_size = (block_num - 1) * TFTP_BLOCK_SIZE + data_len;
        tftp_ctx.state = TFTP_STATE_IDLE;
        printf("TFTP: Transfer complete, %zu bytes\n", tftp_ctx.file_size);
    }
    
    // Send ACK
    tftp_send_ack(block_num);
    
    return 0;
}

/**
 * Send TFTP ACK
 */
static int tftp_send_ack(u16 block_num)
{
    uchar *pkt;
    uchar *p;
    
    pkt = net_tx_packet + net_eth_hdr_size() + IP_UDP_HDR_SIZE;
    p = pkt;
    
    // Opcode: ACK (4)
    *p++ = 0;
    *p++ = 4;
    
    // Block number
    *p++ = (block_num >> 8) & 0xff;
    *p++ = block_num & 0xff;
    
    // Send packet
    net_send_udp_packet(net_server_ip, TFTP_SERVER_PORT,
                        TFTP_CLIENT_PORT, p - pkt);
    
    tftp_ctx.state = TFTP_STATE_ACK;
    
    return 0;
}

/**
 * TFTP timeout handler
 */
static void tftp_timeout_handler(void)
{
    if (tftp_ctx.retries >= TFTP_MAX_RETRIES) {
        printf("TFTP: Max retries exceeded\n");
        tftp_ctx.state = TFTP_STATE_ERROR;
        return;
    }
    
    tftp_ctx.retries++;
    printf("TFTP: Timeout, retry %d\n", tftp_ctx.retries);
    
    // Resend last packet
    if (tftp_ctx.state == TFTP_STATE_RRQ) {
        tftp_send_rrq(tftp_ctx.filename, "octet");
    } else if (tftp_ctx.state == TFTP_STATE_ACK) {
        tftp_send_ack(tftp_ctx.block_num);
    }
}

/**
 * Load file via TFTP
 */
int tftp_load_file(ulong load_addr, const char *filename, ulong server_ip)
{
    ulong start_time;
    int ret;
    
    printf("TFTP: Loading '%s' from %pI4 to 0x%lx\n",
           filename, &server_ip, load_addr);
    
    // Initialize context
    tftp_ctx.server_ip = server_ip;
    tftp_ctx.filename = (char *)filename;
    tftp_ctx.load_addr = load_addr;
    tftp_ctx.block_num = 0;
    tftp_ctx.retries = 0;
    tftp_ctx.state = TFTP_STATE_IDLE;
    
    // Set server IP
    net_server_ip = server_ip;
    
    // Send RRQ
    ret = tftp_send_rrq(filename, "octet");
    if (ret) {
        return ret;
    }
    
    start_time = get_timer(0);
    
    // Wait for data packets
    while (tftp_ctx.state != TFTP_STATE_IDLE &&
           tftp_ctx.state != TFTP_STATE_ERROR) {
        
        // Check timeout
        if (get_timer(start_time) > TFTP_TIMEOUT_MS) {
            tftp_timeout_handler();
            start_time = get_timer(0);
        }
        
        // Process incoming packets
        if (net_loop(TFTP) != 0) {
            printf("TFTP: Network error\n");
            return -1;
        }
    }
    
    if (tftp_ctx.state == TFTP_STATE_ERROR) {
        return -1;
    }
    
    printf("TFTP: File loaded successfully\n");
    
    return 0;
}

