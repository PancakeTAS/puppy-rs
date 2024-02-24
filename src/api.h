/**
 * \file
 * This file interfaces with the nekos.best api to fetch images and gifs.
 */

#pragma once

#include <nekosbest.h>
#include <concord/discord.h>
#include <concord/log.h>
#define DEFAULT_DESCRIPTION "Get a random %s %s"

/** Type of endpoint */
typedef enum {
    PNG, /*!< Endpoint returns a png */
    GIF_NO_TARGET, /*!< Endpoint returns a gif without a target user */
    GIF_TARGET /*!< Endpoint returns a gif with a target user */
} endpoint_type;

/** Data of an endpoint */
typedef struct {
    nekos_endpoint* endpoint; /*!< Endpoint to fetch */
    endpoint_type type; /*!< Type of endpoint */
    char* description; /*!< Description of endpoint */
    char* message; /*!< Message to send when the endpoint is called */
} endpoint_info;

/** Endpoint result */
typedef struct {
    nekos_result *info; /*!< Result of the request */
    nekos_http_response *result; /*!< Http response */
} endpoint_result;

/** Endpoint list */
typedef struct {
    endpoint_info** endpoints; /*!< Array of endpoints */
    int len; /*!< Length of array */
} endpoint_list;

/**
 * Fetch all endpoints from nekos.best.
 * 
 * \param all_endpoints Endpoint list to store data
 * \return 0 on success, -1 on failure
 */
int fetch_endpoints(endpoint_list *all_endpoints);

/**
 * Acquire a random result from an endpoint.
 * 
 * \param result Result to store data
 * \param endpoint Endpoint to fetch
 */
void download_picture(endpoint_result *result, endpoint_info *endpoint);
