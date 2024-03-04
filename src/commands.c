#include "commands.h"

static endpoint_list *all_endpoints;

static void on_interaction(struct discord *client, const struct discord_interaction *event) {
    // find endpoint
    endpoint_info *endpoint = NULL;
    for (int i = 0; i < all_endpoints->len; i++) {
        if (strcmp(all_endpoints->endpoints[i]->name, event->data->name) == 0) {
            endpoint = all_endpoints->endpoints[i];
            break;
        }
    }

    if (!endpoint) {
        log_error("[COMMANDS] Endpoint not found %s!", event->data->name);
        return;
    }

    // fetch result
    endpoint_result bot_result;
    download_picture(&bot_result, endpoint);

    struct discord_attachment attachment = {
        .content = bot_result.file,
        .size = bot_result.file_len,
        .filename = endpoint->type == PNG ? "x.png" : "x.gif"
    };

    struct discord_attachments attachments = {
        .array = &attachment,
        .size = 1
    };

    // send response
    struct discord_interaction_callback_data data = {
        .content = malloc(2001),
        .attachments = &attachments,
    };
    printf("%s", bot_result.message);
    if (endpoint->type == GIF_TARGET)
        snprintf(data.content, 2000, bot_result.message, event->member->user->id, atoll(event->data->options->array[0].value));
    else
        snprintf(data.content, 2000, bot_result.message, event->member->user->id);

    struct discord_interaction_response params = {
        .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
        .data = &data,
    };

    discord_create_interaction_response(client, event->id, event->token, &params, NULL);

    // free resources
    free(bot_result.message);
    free(bot_result.file);
}

void prepare_commands(struct discord *client, u64snowflake app_id, endpoint_list *endpoints) {
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
    for (int i = 0; i < endpoints->len; i++) {
        endpoint_info *info = endpoints->endpoints[i];

        command_params[i].default_permission = true;
        command_params[i].application_id = app_id;
        command_params[i].dm_permission = false;
        command_params[i].type = DISCORD_APPLICATION_CHAT_INPUT;
        command_params[i].description = info->description;
        command_params[i].name = info->name;

        if (info->type == GIF_TARGET)
            command_params[i].options = &options;

        log_info("[COMMANDS] Registering /%s", command_params[i].name);
    }

    // create commands
    struct discord_application_commands commands = {
        .array = command_params,
        .size = endpoints->len,
    };

    discord_bulk_overwrite_global_application_commands(client, app_id, &commands, NULL);
    discord_set_on_interaction_create(client, on_interaction);
    log_info("[COMMANDS] SUCCESS - Created all commands");
}
