/**
 * \file
 * This file manages a cache file for each and every api endpoints
 */

#pragma once

#include <concord/discord.h>
#include <concord/log.h>

#include "api.h"

/// Cache file data
typedef struct {
    char message[2001]; /*!< Message to send with the file */
    char* data; /*!< Data of the file */
    long len; /*!< Length of the file */
} cache_file;

/**
 * Grab file from cache if available
 *
 * \param cache_file The cache file to write to
 * \param endpoint The endpoint to grab the file from
 * \return 0 if the file was grabbed successfully, 1 otherwise
*/
int grab_file(cache_file *cache_file, endpoint_info *endpoint);

/**
 * Ensure all endpoints are up to date
 *
 * \param endpoint List of endpoints
 * \return 0 if the file was written successfully, 1 otherwise
*/
int ensure_cache_validity(endpoint_list *bot_endpoints);

/**
 * Free cache file
 *
 * \param cache_file The cache file to free
 */
void free_cache_file(cache_file *cache_file);
