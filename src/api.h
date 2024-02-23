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
     * Endpoint returns one or more pngs
     */
    PNGS,

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
    size_t len;
    
} endpoint_list;

/**
 * Fetch all endpoints from nekos.best.
 * 
 * \param client Discord client
 * \param list List to store endpoints
 * \return 0 on success, -1 on failure
 */
int fetch_endpoints(struct discord *client, endpoint_list *list);
