/**
 * Root CA and Certificate Management
 * Author: jk1806
 * Created: 2024-04-25
 * 
 * Root Certificate Authority management
 * Handles certificate chain validation and trust anchors
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/crypto.h>
#include <linux/x509_cert_parser.h>

#define ROOT_CA_VERSION "1.0.0"
#define MAX_ROOT_CAS 16
#define MAX_CERT_SIZE 4096

struct root_ca_entry {
    struct x509_cert *cert;
    char name[64];
    bool trusted;
    u32 usage_flags;
    time_t expiry;
};

static struct root_ca_entry root_cas[MAX_ROOT_CAS];
static int root_ca_count = 0;

/**
 * Add root CA certificate
 */
int root_ca_add(const u8 *cert_data, size_t cert_len, const char *name)
{
    struct x509_cert *cert;
    int ret;
    
    if (root_ca_count >= MAX_ROOT_CAS) {
        pr_err("Root CA: Maximum CAs reached\n");
        return -ENOSPC;
    }
    
    if (cert_len > MAX_CERT_SIZE) {
        pr_err("Root CA: Certificate too large\n");
        return -EINVAL;
    }
    
    // Parse certificate
    cert = x509_cert_parse(cert_data, cert_len);
    if (!cert) {
        pr_err("Root CA: Failed to parse certificate\n");
        return -EINVAL;
    }
    
    // Verify it's a CA certificate
    if (!x509_cert_is_ca(cert)) {
        pr_err("Root CA: Not a CA certificate\n");
        x509_free_certificate(cert);
        return -EINVAL;
    }
    
    // Add to list
    root_cas[root_ca_count].cert = cert;
    strncpy(root_cas[root_ca_count].name, name, sizeof(root_cas[root_ca_count].name) - 1);
    root_cas[root_ca_count].trusted = true;
    root_cas[root_ca_count].usage_flags = X509_CERT_USAGE_CA;
    root_cas[root_ca_count].expiry = x509_cert_get_expiry(cert);
    
    root_ca_count++;
    
    pr_info("Root CA: Added '%s', expires: %lld\n", name, root_cas[root_ca_count - 1].expiry);
    
    return 0;
}

/**
 * Verify certificate chain
 */
int root_ca_verify_chain(struct x509_cert *leaf_cert)
{
    struct x509_cert *cert;
    struct x509_cert *issuer;
    int i;
    int ret;
    
    cert = leaf_cert;
    
    // Walk up the chain
    while (cert) {
        // Find issuer
        issuer = NULL;
        for (i = 0; i < root_ca_count; i++) {
            if (x509_cert_match_issuer(cert, root_cas[i].cert)) {
                issuer = root_cas[i].cert;
                break;
            }
        }
        
        if (!issuer) {
            pr_err("Root CA: Issuer not found in trust store\n");
            return -ENOENT;
        }
        
        // Verify signature
        ret = x509_cert_verify_signature(cert, issuer);
        if (ret) {
            pr_err("Root CA: Signature verification failed\n");
            return ret;
        }
        
        // Check if we reached a root CA
        if (x509_cert_is_self_signed(issuer)) {
            // Found root CA
            for (i = 0; i < root_ca_count; i++) {
                if (root_cas[i].cert == issuer) {
                    if (!root_cas[i].trusted) {
                        pr_err("Root CA: Root CA not trusted\n");
                        return -EACCES;
                    }
                    pr_info("Root CA: Chain verified to trusted root '%s'\n",
                            root_cas[i].name);
                    return 0;
                }
            }
        }
        
        cert = issuer;
    }
    
    return -EINVAL;
}

/**
 * Check certificate expiry
 */
int root_ca_check_expiry(void)
{
    time_t now = ktime_get_real_seconds();
    int i;
    int expired_count = 0;
    
    for (i = 0; i < root_ca_count; i++) {
        if (root_cas[i].expiry < now) {
            pr_warn("Root CA: '%s' has expired\n", root_cas[i].name);
            root_cas[i].trusted = false;
            expired_count++;
        }
    }
    
    if (expired_count > 0) {
        pr_warn("Root CA: %d certificates expired\n", expired_count);
    }
    
    return expired_count;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("Root CA Certificate Management");
MODULE_VERSION(ROOT_CA_VERSION);

