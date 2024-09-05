/**
 * SBOM (Software Bill of Materials) Generation
 * Author: jk1806
 * Created: 2024-10-05
 * 
 * Generates SBOM for compliance and security tracking
 * Supports SPDX and CycloneDX formats
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>

#define SBOM_VERSION "1.0.0"

struct sbom_component {
    char name[64];
    char version[32];
    char license[64];
    char supplier[64];
    char purl[128];  // Package URL
};

static struct sbom_component components[64];
static int component_count = 0;

/**
 * Add component to SBOM
 */
int sbom_add_component(const char *name, const char *version,
                       const char *license, const char *supplier)
{
    if (component_count >= 64) {
        return -ENOSPC;
    }
    
    strncpy(components[component_count].name, name, 63);
    strncpy(components[component_count].version, version, 31);
    strncpy(components[component_count].license, license, 63);
    strncpy(components[component_count].supplier, supplier, 63);
    
    snprintf(components[component_count].purl, sizeof(components[component_count].purl),
             "pkg:generic/%s@%s", name, version);
    
    component_count++;
    
    return 0;
}

/**
 * Generate SPDX format SBOM
 */
int sbom_generate_spdx(char *buffer, size_t buffer_size)
{
    int len = 0;
    int i;
    
    len += snprintf(buffer + len, buffer_size - len,
                    "SPDXVersion: SPDX-2.3\n");
    len += snprintf(buffer + len, buffer_size - len,
                    "DataLicense: CC0-1.0\n");
    len += snprintf(buffer + len, buffer_size - len,
                    "SPDXID: SPDXRef-DOCUMENT\n");
    len += snprintf(buffer + len, buffer_size - len,
                    "DocumentName: Embedded System SBOM\n");
    len += snprintf(buffer + len, buffer_size - len,
                    "DocumentNamespace: https://example.com/sbom\n");
    len += snprintf(buffer + len, buffer_size - len,
                    "Creator: Tool: SBOM-Generator-%s\n", SBOM_VERSION);
    len += snprintf(buffer + len, buffer_size - len, "\n");
    
    for (i = 0; i < component_count; i++) {
        len += snprintf(buffer + len, buffer_size - len,
                        "PackageName: %s\n", components[i].name);
        len += snprintf(buffer + len, buffer_size - len,
                        "SPDXID: SPDXRef-Package-%d\n", i);
        len += snprintf(buffer + len, buffer_size - len,
                        "PackageVersion: %s\n", components[i].version);
        len += snprintf(buffer + len, buffer_size - len,
                        "PackageLicenseDeclared: %s\n", components[i].license);
        len += snprintf(buffer + len, buffer_size - len,
                        "PackageSupplier: %s\n", components[i].supplier);
        len += snprintf(buffer + len, buffer_size - len, "\n");
    }
    
    return len;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("SBOM Generation");
MODULE_VERSION(SBOM_VERSION);

