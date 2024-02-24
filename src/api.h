/**
 * \file
 * This file interfaces with the nekos.best api to fetch images and gifs.
 */

#pragma once

#include <nekosbest.h>
#include <concord/discord.h>
#include <concord/log.h>
#define DEFAULT_DESCRIPTION "Get a random %s %s"

/**
 * Type of endpoint
 */
typedef enum {

    /**
     * Endpoint returns a png
     */
    PNG,

    /**
     * Endpoint returns a gif without a target user
     */
    GIF_NO_TARGET,

    /**
     * Endpoint returns a gif with a target user
     */
    GIF_TARGET

} endpoint_type;

/**
 * Data of an endpoint
 */
typedef struct {

    /**
     * Endpoint to fetch
     */
    nekos_endpoint* endpoint;

    /**
     * Description of endpoint.
     */
    char* description;

    /**
     * Type of endpoint
     */
    endpoint_type type;

    /**
     * Message to send when the endpoint is called. 
     */
    char* message;

} endpoint_info;

/**
 * Endpoint result
 */
typedef struct {

    /**
     * Nekos result
     */
    nekos_result *infos;

    /**
     * Http response
     */
    nekos_http_response *results;

} endpoint_result;

/**
 * Endpoint list
 */
typedef struct {

    /**
     * Array of endpoints
     */
    endpoint_info** endpoints;

    /**
     * Length of array 
     */
    int len;
    
} endpoint_list;

/**
 * Fetch all endpoints from nekos.best.
 * 
 * \param client Discord client
 * \param list List to store endpoints
 * \return 0 on success, -1 on failure
 */
int fetch_endpoints(struct discord *client, endpoint_list *list);

/**
 * Acquire a random result from an endpoint.
 * 
 * \param result Result to store data
 * \param endpoint Endpoint to fetch
 */
void download_picture(endpoint_result *result, endpoint_info *endpoint);
