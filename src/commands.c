#include "commands.h"

static endpoint_list *all_endpoints;

static void on_interaction(struct discord *client, const struct discord_interaction *event) {
    // find endpoint
    endpoint_info *endpoint = NULL;
    for (int i = 0; i < all_endpoints->len; i++) {
        if (strcmp(all_endpoints->endpoints[i]->endpoint->name, event->data->name) == 0) {
            endpoint = all_endpoints->endpoints[i];
            break;
        }
    }

    if (!endpoint) {
        log_error("[COMMANDS] Endpoint not found %s!", event->data->name);
        return;
    }

    // fetch result
    endpoint_result result;
    download_picture(&result, endpoint);

    struct discord_attachment attachment = {
        .content = result.result->text,
        .size = result.result->len,
        .filename = endpoint->type == PNG ? "x.png" : "x.gif"
    };

    struct discord_attachments attachments = {
        .array = &attachment,
        .size = 1
    };

    // create response text
    char* text = (char*) malloc(2001);
    char* text_ptr = text;
    if (endpoint->type == GIF_TARGET) {
        text_ptr += snprintf(text_ptr, 2001, endpoint->message, event->member->user->id, atoll(event->data->options->array->value));
        snprintf(text_ptr, 2001, "\n\nAnime: %s", result.info->source.gif->anime_name);
    } else if (endpoint->type == GIF_NO_TARGET) {
        text_ptr += snprintf(text_ptr, 2001, endpoint->message, event->member->user->id);
        snprintf(text_ptr, 2001, "\n\nAnime: %s", result.info->source.gif->anime_name);
    } else {
        text_ptr += snprintf(text_ptr, 2001, endpoint->message, event->member->user->id);
        sprintf(text_ptr, "\n\nArtist:\n%s: <%s>\n\nSource:\n<%s>", result.info->source.png->artist_name, result.info->source.png->artist_href, result.info->source.png->source_url);
    }

    // send response
    struct discord_interaction_callback_data data = {
        .content = text,
        .attachments = &attachments,
    };

    struct discord_interaction_response params = {
        .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
        .data = &data,
    };
    discord_create_interaction_response(client, event->id, event->token, &params, NULL);

    // free resources
    free(text);
    free(result.info->url);
    if (endpoint->type == PNG) {
        free(result.info->source.png->artist_name);
        free(result.info->source.png->artist_href);
        free(result.info->source.png->source_url);
        free(result.info->source.png);
    } else {
        free(result.info->source.gif->anime_name);
        free(result.info->source.gif);
    }
    free(result.info);
    free(result.result->text);
    free(result.result);
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
    struct discord_application_command command_params[endpoints->len];
    memset(command_params, 0, sizeof(command_params));
    for (int i = 0; i < endpoints->len; i++) {
        endpoint_info *info = endpoints->endpoints[i];

        command_params[i].default_permission = true;
        command_params[i].application_id = app_id;
        command_params[i].dm_permission = false;
        command_params[i].type = DISCORD_APPLICATION_CHAT_INPUT;
        command_params[i].description = info->description;
        command_params[i].name = info->endpoint->name;

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
