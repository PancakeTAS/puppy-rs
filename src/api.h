/// \file api.h This file interfaces with multiple apis to fetch images and gifs.

#pragma once

#include <nekosbest.h>

/// Default description for endpoints
#define DEFAULT_DESCRIPTION "Get a random %s %s"

typedef enum {
    PNG, //!< Endpoint returns a png
    GIF_NO_TARGET, //!< Endpoint returns a gif without a target user
    GIF_TARGET //!< Endpoint returns a gif with a target user
} endpoint_type; //!< Type of endpoint

typedef struct {
    char* name; //!< Name of endpoint to fetch
    nekos_format format; //!< Format of the endpoint
    int api; //!< API to fetch from (bit 1: nekos.best, bit 2: otakugifs.xyz)

    endpoint_type type; //!< Type of endpoint
    char description[64]; //!< Description of endpoint
    char message[2001]; //!< Message to send when the endpoint is called
} endpoint_info; //!< Endpoint information

typedef struct {
    endpoint_info* endpoints; //!< Array of endpoints
    int len; //!< Length of array
} endpoint_list; //!< List of endpoints

typedef struct {
    char message[2001]; //!< Formatted response message
    char* file; //!< Data of the file
    int file_len; //!< Length of the file
} endpoint_result; //!< Result of an endpoint

/**
 * Fetch all endpoints from all apis.
 *
 * \param all_endpoints
 *   Endpoint list to store data
 *
 * \return
 *   0 on success, 1 on fatal failure. The application is expected to exit on failure, as no cleanup is performed.
 */
int fetch_endpoints(endpoint_list *all_endpoints);

/**
 * Acquire a random result from an endpoint.
 *
 * \param result
 *   Result to store data
 * \param endpoint
 *   Endpoint to fetch
 *
 * \return
 *   0 on success, 1 on failure.
 */
int download_picture(endpoint_result *result, endpoint_info *endpoint);

/**
 * Free the memory of an endpoint list.
 *
 * \param list
 *   Endpoint list to free
 */
void free_endpoints(endpoint_list *list);

/**
 * Free the memory of an endpoint result.
 *
 * \param result
 *   Endpoint result to free
 */
void free_result(endpoint_result *result);
