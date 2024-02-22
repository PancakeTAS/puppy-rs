/**
 * \file
 * This class interfaces with the nekos.best api to fetch images and gifs.
 */

#pragma once

#include <nekosbest.h>
#include <concord/log.h>
#define DEFAULT_COMMAND_DESCRIPTION "Get a random %s %s"

/**
 * Data required for creating a command
 */
typedef struct {

    /**
     * Endpoint to use for command
     */
    nekos_endpoint* endpoint;

    /**
     * Description of command. Leave empty to use \link DEFAULT_COMMAND_DESCRIPTION \endlink
     */
    char* command_description;

    /**
     * Name of command. Leave empty to use nekos_endpoint->name
     */
    char* command_name;

    /**
     * Response message to send. If NULL, then the command is not available without specifying a user.
     */
    char* response_message;

    /**
     * Response message to send when user is specified. If NULL, then the command is not available while specifying a user.
     */
    char* response_message_user;

} command_data;

/**
 * Fetch all endpoints from nekos.best and register them as commands.
 * 
 * \param register_callback Callback to register commands
 * \return
 *  0 Success \n
 *  1 Failure
 */
int fetch_endpoints(void (*register_callback)(command_data*));