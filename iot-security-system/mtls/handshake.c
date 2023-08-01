#include <stdio.h>
#include <openssl/ssl.h>

int perform_mtls_handshake(SSL *ssl)
{
    int ret = SSL_connect(ssl);
    
    if (ret <= 0) {
        int err = SSL_get_error(ssl, ret);
        fprintf(stderr, "mTLS handshake failed: %d\n", err);
        return -1;
    }
    
    /* Verify peer certificate */
    X509 *peer_cert = SSL_get_peer_certificate(ssl);
    if (!peer_cert) {
        fprintf(stderr, "No peer certificate\n");
        return -1;
    }
    
    /* Verify certificate chain */
    long verify_result = SSL_get_verify_result(ssl);
    if (verify_result != X509_V_OK) {
        fprintf(stderr, "Certificate verification failed: %s\n",
                X509_verify_cert_error_string(verify_result));
        X509_free(peer_cert);
        return -1;
    }
    
    X509_free(peer_cert);
    return 0;
}

