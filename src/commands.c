#include "commands.h"

#include <errno.h>

static endpoint_list *all_endpoints;

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
        log_error("[COMMANDS] Endpoint not found %s!", event->data->name);
        return;
    }

    // fetch result from cache
    cache_file bot_cache;
    int i = grab_file(&bot_cache, endpoint);
    if (i) {
        log_error("[COMMANDS] Failed to fetch %s: %d", endpoint->name, i);
        return;
    }

    char message[2001];
    if (endpoint->type == GIF_TARGET)
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
    log_info("[COMMANDS] Succesfully processed /%s", endpoint->name);

    // free resources
    free_cache_file(&bot_cache);

    // ensure cache validity
    if (ensure_cache_validity(all_endpoints)) {
        log_fatal("[COMMANDS] Failed to re-ensure cache validity");

        discord_shutdown(client);
        return;
    }
    log_info("[COMMANDS] Re-ensured cache validity");
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
        log_trace("[COMMANDS] calloc() failed: %d", strerror(errno));

        return 1;
    }
    log_trace("[COMMANDS] calloc() success");

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

        log_debug("[COMMANDS] Created command /%s", command_params[i].name);
    }

    // create commands
    discord_set_on_interaction_create(client, on_interaction);
    discord_bulk_overwrite_global_application_commands(client, app_id, &(struct discord_application_commands) {
        .array = command_params,
        .size = endpoints->len,
    }, NULL);
    log_info("[COMMANDS] Successfully created %d commands", endpoints->len);

    // cleanup
    free(command_params);

    return 0;
}
