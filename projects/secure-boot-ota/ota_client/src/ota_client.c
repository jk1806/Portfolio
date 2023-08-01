#include "ota.h"
#include "boot_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

// v1.1 - Added better error handling (2024-05-15)
// v1.0 - Initial OTA client implementation

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        return 0;
    }
    
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}

int ota_download_firmware(const char *server_url, const char *version,
                         uint8_t **firmware_data, size_t *firmware_size)
{
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;
    char url[512];
    
    chunk.memory = malloc(1);
    chunk.size = 0;
    
    curl = curl_easy_init();
    if (!curl) {
        return -1;
    }
    
    /* Build URL - TODO: Add URL validation */
    snprintf(url, sizeof(url), "%s/api/firmware/download?version=%s",
             server_url, version ? version : "latest");
    
    // FIXME: URL size check - 512 might not be enough
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, OTA_DOWNLOAD_TIMEOUT_MS / 1000);
    
    res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        free(chunk.memory);
        curl_easy_cleanup(curl);
        return -1;
    }
    
    *firmware_data = (uint8_t *)chunk.memory;
    *firmware_size = chunk.size;
    
    curl_easy_cleanup(curl);
    return 0;
}

int ota_verify_firmware(const uint8_t *firmware, size_t size,
                       const uint8_t *signature, size_t sig_len)
{
    /* Verify signature using crypto library */
    extern uint8_t boot_public_key[];
    
    return crypto_verify_rsa(firmware, size, signature, sig_len,
                           boot_public_key, BOOT_PUBLIC_KEY_SIZE);
}

int ota_install_firmware(const uint8_t *firmware, size_t size)
{
    FILE *fp;
    const char *firmware_path = "/tmp/firmware_update.bin";
    
    /* Write firmware to temporary location */
    fp = fopen(firmware_path, "wb");
    if (!fp) {
        return -1;
    }
    
    if (fwrite(firmware, 1, size, fp) != size) {
        fclose(fp);
        return -1;
    }
    
    fclose(fp);
    
    /* TODO: Flash firmware to device */
    printf("[OTA] Firmware downloaded to %s\n", firmware_path);
    printf("[OTA] Ready to flash firmware\n");
    
    return 0;
}

