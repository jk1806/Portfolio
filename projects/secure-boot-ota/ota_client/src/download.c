#include "ota.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// v1.0 - Initial implementation
// Simple file download using curl

int ota_download_file(const char *url, const char *output_path)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;
    
    curl = curl_easy_init();
    if (!curl) {
        return -1;
    }
    
    fp = fopen(output_path, "wb");
    if (!fp) {
        curl_easy_cleanup(curl);
        return -1;
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    // TODO: Add timeout, SSL verification, etc.
    
    res = curl_easy_perform(curl);
    
    fclose(fp);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        return -1;
    }
    
    return 0;
}

