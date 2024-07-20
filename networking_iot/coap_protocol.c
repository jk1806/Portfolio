/**
 * CoAP (Constrained Application Protocol) Implementation
 * Author: jk1806
 * Created: 2024-07-01
 * 
 * RFC 7252 CoAP protocol for IoT devices
 * Supports GET, POST, PUT, DELETE methods
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/udp.h>
#include <linux/inet.h>

#define COAP_VERSION "1.0.0"
#define COAP_DEFAULT_PORT 5683
#define COAP_MAX_PAYLOAD 1024

struct coap_header {
    u8 ver_t_tkl;
    u8 code;
    u16 message_id;
    u8 token[8];
    u8 token_len;
    u8 options[256];
    u16 options_len;
    u8 payload[COAP_MAX_PAYLOAD];
    u16 payload_len;
};

static u16 coap_message_id = 1;

/**
 * Build CoAP header
 */
static int coap_build_header(struct coap_header *hdr, u8 code,
                             const u8 *token, u8 token_len)
{
    if (token_len > 8) {
        pr_err("CoAP: Token too long\n");
        return -1;
    }
    
    hdr->ver_t_tkl = (1 << 6) | (token_len & 0x0f);  // Version 1, token length
    hdr->code = code;
    hdr->message_id = coap_message_id++;
    hdr->token_len = token_len;
    
    if (token && token_len > 0) {
        memcpy(hdr->token, token, token_len);
    }
    
    hdr->options_len = 0;
    hdr->payload_len = 0;
    
    return 0;
}

/**
 * Send CoAP request
 */
int coap_send_request(struct sockaddr_in *server, u8 method,
                      const char *uri, const u8 *payload, size_t payload_len)
{
    struct coap_header hdr;
    struct sk_buff *skb;
    u8 *data;
    int len;
    
    // Build header
    coap_build_header(&hdr, method, NULL, 0);
    
    // Add URI option
    coap_add_option(&hdr, COAP_OPTION_URI_PATH, uri, strlen(uri));
    
    // Add payload
    if (payload && payload_len > 0) {
        if (payload_len > COAP_MAX_PAYLOAD) {
            pr_err("CoAP: Payload too large\n");
            return -1;
        }
        memcpy(hdr.payload, payload, payload_len);
        hdr.payload_len = payload_len;
    }
    
    // Build packet
    len = 4 + hdr.token_len + hdr.options_len + hdr.payload_len;
    
    skb = alloc_skb(len, GFP_KERNEL);
    if (!skb) {
        return -1;
    }
    
    data = skb_put(skb, len);
    
    // Header
    *data++ = hdr.ver_t_tkl;
    *data++ = hdr.code;
    put_unaligned_be16(hdr.message_id, data);
    data += 2;
    
    // Token
    if (hdr.token_len > 0) {
        memcpy(data, hdr.token, hdr.token_len);
        data += hdr.token_len;
    }
    
    // Options
    if (hdr.options_len > 0) {
        memcpy(data, hdr.options, hdr.options_len);
        data += hdr.options_len;
    }
    
    // Payload marker
    if (hdr.payload_len > 0) {
        *data++ = 0xff;
        memcpy(data, hdr.payload, hdr.payload_len);
    }
    
    // Send UDP packet
    udp_send_skb(skb, server);
    
    pr_info("CoAP: Request sent, method %d, URI %s\n", method, uri);
    
    return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("CoAP Protocol Implementation");
MODULE_VERSION(COAP_VERSION);

