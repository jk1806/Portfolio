#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

int load_device_certificate(const char *cert_file, X509 **cert)
{
    FILE *fp = fopen(cert_file, "r");
    if (!fp) {
        return -1;
    }
    
    *cert = PEM_read_X509(fp, NULL, NULL, NULL);
    fclose(fp);
    
    if (!*cert) {
        return -1;
    }
    
    return 0;
}

int verify_device_certificate(X509 *cert, X509_STORE *store)
{
    X509_STORE_CTX *ctx = X509_STORE_CTX_new();
    if (!ctx) {
        return -1;
    }
    
    if (!X509_STORE_CTX_init(ctx, store, cert, NULL)) {
        X509_STORE_CTX_free(ctx);
        return -1;
    }
    
    int ret = X509_verify_cert(ctx);
    X509_STORE_CTX_free(ctx);
    
    return (ret == 1) ? 0 : -1;
}

