/**
 * \file
 * This file interfaces with the nekos.best api to fetch images and gifs.
 */

#pragma once

#include <nekosbest.h>
#define DEFAULT_DESCRIPTION "Get a random %s %s"

/** Type of endpoint */
typedef enum {
    PNG, /*!< Endpoint returns a png */
    GIF_NO_TARGET, /*!< Endpoint returns a gif without a target user */
    GIF_TARGET /*!< Endpoint returns a gif with a target user */
} endpoint_type;

/** Data of an endpoint */
typedef struct {
    char* name; /*!< Name of endpoint to fetch */
    nekos_format format; /*!< Format of the endpoint */

    endpoint_type type; /*!< Type of endpoint */
    char description[64]; /*!< Description of endpoint */
    char message[2001]; /*!< Message to send when the endpoint is called */
} endpoint_info;

/** Endpoint list */
typedef struct {
    endpoint_info* endpoints; /*!< Array of endpoints */
    int len; /*!< Length of array */
} endpoint_list;

/** Endpoint result */
typedef struct {
    char message[2001]; /*!< Formatted response message */
    char* file; /*!< Data of the file */
    int file_len; /*!< Length of the file */
} endpoint_result;

/**
 * Fetch all endpoints from nekos.best.
 *
 * \param all_endpoints Endpoint list to store data
 * \return 0 on success, 1 on failure
 */
int fetch_endpoints(endpoint_list *all_endpoints);

/**
 * Acquire a random result from an endpoint.
 *
 * \param result Result to store data
 * \param endpoint Endpoint to fetch
 */
int download_picture(endpoint_result *result, endpoint_info *endpoint);

/**
 * Free the memory of an endpoint list.
 *
 * \param list Endpoint list to free
 */
void free_endpoints(endpoint_list *list);

/**
 * Free the memory of an endpoint result.
 *
 * \param result Endpoint result to free
 */
void free_result(endpoint_result *result);
