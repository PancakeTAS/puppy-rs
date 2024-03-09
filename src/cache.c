#include "cache.h"

#define CACHE_DIR "cache"

#include "api.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

int grab_file(cache_file *cache_file, endpoint_info *endpoint) {
    // ensure cache exists
    struct stat st = {0};
    if (stat(CACHE_DIR, &st) == -1) {
        mkdir(CACHE_DIR, 0700);
    }

    // ensure endpoint is cached
    char filename[64];
    char messagefilename[64];
    sprintf(filename, "%s/%s.%s", CACHE_DIR, endpoint->name, endpoint->type == PNG ? "png" : "gif");
    sprintf(messagefilename, "%s/%s.txt", CACHE_DIR, endpoint->name);
    if (stat(filename, &st) == -1 || stat(messagefilename, &st) == -1) {
        log_trace("[CACHE] stat() failed: %s", strerror(errno));
        return 1;
    }
    log_trace("[CACHE] stat() success: %p", filename);

    // open cache file
    FILE* file = fopen(filename, "r");
    FILE* messagefile = fopen(messagefilename, "r");
    if (!file || !messagefile) {
        log_trace("[CACHE] fopen() failed: %s", strerror(errno));
        return 1;
    }
    log_trace("[CACHE] fopen() success: %p", file);

    // create cache file buffer
    fseek(file, 0, SEEK_END);
    cache_file->len = ftell(file);
    fseek(file, 0, SEEK_SET);
    cache_file->data = malloc(cache_file->len);
    memset(cache_file->message, 0, 2001);
    if (!cache_file->data) {
        log_trace("[CACHE] malloc() failed: %s", strerror(errno));
        fclose(file);
        return 1;
    }
    log_trace("[CACHE] malloc() success");

    // read cache file
    fread(cache_file->data, cache_file->len, 1, file);
    fclose(file);
    fread(cache_file->message, 2001, 1, messagefile);
    fclose(messagefile);

    // delete cache file
    remove(filename);
    remove(messagefilename);

    return 0;
}

int ensure_cache_validity(endpoint_list *bot_endpoints) {
    // ensure cache exists
    struct stat st = {0};
    if (stat(CACHE_DIR, &st) == -1)
        mkdir(CACHE_DIR, 0700);

    // check if cache is valid
    char filename[64];
    char messagefilename[64];
    for (int i = 0; i < bot_endpoints->len; i++) {
        sprintf(filename, "%s/%s.%s", CACHE_DIR, bot_endpoints->endpoints[i].name, bot_endpoints->endpoints[i].type == PNG ? "png" : "gif");
        sprintf(messagefilename, "%s/%s.txt", CACHE_DIR, bot_endpoints->endpoints[i].name);

        // check if endpoint has a cache
        if (!stat(filename, &st))
            continue;

        // open new cache
        FILE* file = fopen(filename, "w");
        FILE* messagefile = fopen(messagefilename, "w");
        if (!file || !messagefile) {
            log_trace("[CACHE] fopen() failed: %s", strerror(errno));
            return 1;
        }
        log_trace("[CACHE] fopen() success");

        // fetch image from api
        endpoint_result bot_result;
        int status = download_picture(&bot_result, &bot_endpoints->endpoints[i]);
        if (status) {
            log_trace("[CACHE] download_picture() failed: %d", status);

            fclose(file);
            fclose(messagefile);
            return 1;
        }
        log_trace("[CACHE] download_picture() success");

        // write to cache
        fwrite(bot_result.file, bot_result.file_len, 1, file);
        fclose(file);
        fwrite(bot_result.message, strlen(bot_result.message), 1, messagefile);
        fclose(messagefile);

        // free
        free_result(&bot_result);

        log_info("[CACHE] Fetched new %s result in cache", bot_endpoints->endpoints[i].name);
    }

    return 0;
}

void free_cache_file(cache_file *cache_file) {
    free(cache_file->data);
}
