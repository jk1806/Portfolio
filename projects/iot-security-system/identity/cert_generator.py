#!/usr/bin/env python3
"""Generate X.509 certificates for IoT device identity
v1.2 - Added SAN extension (2024-06-20)
v1.1 - Fixed certificate validity period
v1.0 - Initial implementation
"""

from cryptography import x509
from cryptography.x509.oid import NameOID
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.backends import default_backend
import datetime

def generate_device_certificate(device_id, ca_key, ca_cert):
    """Generate a device certificate signed by CA
    Args:
        device_id: Unique device identifier
        ca_key: CA private key
        ca_cert: CA certificate
    Returns:
        (certificate, device_private_key) tuple
    """
    # Generate device key - RSA-2048 (could use ECDSA for smaller keys)
    device_key = rsa.generate_private_key(
        public_exponent=65537,  # Standard RSA exponent
        key_size=2048,  # 2048 bits - good balance of security/performance
        backend=default_backend()
    )
    
    # Create certificate subject - device identity
    subject = x509.Name([
        x509.NameAttribute(NameOID.COUNTRY_NAME, "US"),  # TODO: Make configurable
        x509.NameAttribute(NameOID.STATE_OR_PROVINCE_NAME, "CA"),
        x509.NameAttribute(NameOID.ORGANIZATION_NAME, "IoT Devices"),
        x509.NameAttribute(NameOID.COMMON_NAME, f"device-{device_id}"),
    ])
    
    # Build certificate - valid for 1 year
    cert = x509.CertificateBuilder().subject_name(
        subject
    ).issuer_name(
        ca_cert.subject  # Signed by CA
    ).public_key(
        device_key.public_key()
    ).serial_number(
        x509.random_serial_number()  # Random serial to prevent collisions
    ).not_valid_before(
        datetime.datetime.utcnow()
    ).not_valid_after(
        datetime.datetime.utcnow() + datetime.timedelta(days=365)  # 1 year validity
    ).add_extension(
        x509.SubjectAlternativeName([
            x509.DNSName(f"device-{device_id}.local"),  # mDNS name
        ]),
        critical=False,
    ).sign(ca_key, hashes.SHA256(), default_backend())
    
    return cert, device_key

if __name__ == '__main__':
    print("Device certificate generator")
    # TODO: Add CLI interface
    # TODO: Add certificate revocation list (CRL) support
    # Implementation continues...

