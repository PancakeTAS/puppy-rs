#include "commands.h"
#include "log.h"
#include "cache.h"

#include <errno.h>

static endpoint_list *all_endpoints;

/**
 * Handle interaction
 *
 * \param client
 *   The discord client
 *
 * \param event
 *   The interaction event
 */
static void on_interaction(struct discord *client, const struct discord_interaction *event) {

    // find endpoint
    endpoint_info *endpoint = NULL;
    for (int i = 0; i < all_endpoints->len; i++) {
        if (strcmp(all_endpoints->endpoints[i].name, event->data->name) == 0) {
            endpoint = &all_endpoints->endpoints[i];
            break;
        }
    }

    if (!endpoint) {
        log_error("COMMANDS", "Failed to find endpoint %s", event->data->name);
        return;
    }
    log_trace("COMMANDS", "User requested endpoint %s", endpoint->name);


    // fetch result from cache
    cache_file bot_cache;
    int i = grab_file(&bot_cache, endpoint);
    if (i) {
        log_error("COMMANDS", "grab_file() failed: Unable to fetch %s from cache, reensuring cache validity", endpoint->name);

        if (ensure_cache_validity(all_endpoints))
            log_error("COMMANDS", "Failed to re-ensure cache validity, this is bad");
        return;
    }
    log_debug("COMMANDS", "grab_file() success: Fetched %s from cache", endpoint->name);

    char message[2001];
    if (endpoint->type == GIF_TARGET && event->data->options->size > 0 && event->data->options->array[0].value)
        snprintf(message, 2000, bot_cache.message, event->member->user->id, atoll(event->data->options->array[0].value));
    else
        snprintf(message, 2000, bot_cache.message, event->member->user->id);


    // send response
    discord_create_interaction_response(client, event->id, event->token, &(struct discord_interaction_response) {
        .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
        .data = &(struct discord_interaction_callback_data) {
            .content = message,
            .attachments = &(struct discord_attachments) {
                .array = &(struct discord_attachment) {
                    .content = bot_cache.data,
                    .size = bot_cache.len,
                    .filename = endpoint->type == PNG ? "x.png" : "x.gif"
                },
                .size = 1
            }
        }
    }, NULL);
    log_trace("COMMANDS", "discord_create_interaction_response() success: Sent response to user");

    // free resources
    free_cache_file(&bot_cache);

    log_info("COMMANDS", "Successfully processed %s endpoint request from %s", endpoint->name, event->member->user->username);

    // ensure cache validity
    if (ensure_cache_validity(all_endpoints)) {
        log_error("COMMANDS", "Failed to re-ensure cache validity, this is bad");
        return;
    }
    log_trace("COMMANDS", "ensure_cache_validity() success: Cache validated");

}

int prepare_commands(struct discord *client, u64snowflake app_id, endpoint_list *endpoints) {
    all_endpoints = endpoints;

    // create command options
    struct discord_application_command_options options = {
        .size = 1,
        .array = &(struct discord_application_command_option) {
            .type = DISCORD_APPLICATION_OPTION_USER,
            .name = "user",
            .description = "User to target",
            .required = true
        }
    };

    // create command params
    struct discord_application_command* command_params = calloc(endpoints->len, sizeof(struct discord_application_command));
    if (!command_params) {
        log_fatal("COMMANDS", "calloc() failed: %s", strerror(errno));
        return 1;
    }
    log_trace("COMMANDS", "calloc() success: Command parameters created");

    for (int i = 0; i < endpoints->len; i++) {
        endpoint_info *info = &endpoints->endpoints[i];

        command_params[i].default_permission = true;
        command_params[i].application_id = app_id;
        command_params[i].dm_permission = false;
        command_params[i].type = DISCORD_APPLICATION_CHAT_INPUT;
        command_params[i].description = info->description;
        command_params[i].name = info->name;

        if (info->type == GIF_TARGET)
            command_params[i].options = &options;

        log_debug("COMMANDS", "Initialized command parameters for /%s", info->name);
    }

    // create commands
    discord_set_on_interaction_create(client, on_interaction);
    discord_bulk_overwrite_global_application_commands(client, app_id, &(struct discord_application_commands) {
        .array = command_params,
        .size = endpoints->len,
    }, NULL);
    log_trace("COMMANDS", "discord_bulk_overwrite_global_application_commands() success: Commands created");

    // cleanup
    free(command_params);

    log_debug("COMMANDS", "Successfully initialized %d commands", endpoints->len);

    return 0;
}
