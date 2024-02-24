#include "commands.h"

static endpoint_list *all_endpoints = NULL;

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
        log_error("Endpoint not found %s!", event->data->name);
        return;
    }

    // fetch results
    endpoint_result results;
    download_picture(&results, endpoint);

    struct discord_attachment attachment = {
        .content = results.results[0].text,
        .size = results.results[0].len,
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
        text_ptr += snprintf(text_ptr, 2001, endpoint->message, event->member->user->id, atoll(event->data->options->array[0].value));
        snprintf(text_ptr, 2001, "\n\nAnime: %s", results.infos[0].source.gif->anime_name);
    } else if (endpoint->type == GIF_NO_TARGET) {
        text_ptr += snprintf(text_ptr, 2001, endpoint->message, event->member->user->id);
        snprintf(text_ptr, 2001, "\n\nAnime: %s", results.infos[0].source.gif->anime_name);
    } else {
        text_ptr += snprintf(text_ptr, 2001, endpoint->message, event->member->user->id);
        sprintf(text_ptr, "\n\nArtist:\n%s: <%s>\nSource:\n<%s>", results.infos[0].source.png->artist_name, results.infos[0].source.png->artist_href, results.infos[0].source.png->source_url);
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

        log_info("Created command %s", command_params[i].name);
    }

    // create commands
    struct discord_application_commands commands = {
        .array = command_params,
        .size = endpoints->len,
    };

    discord_bulk_overwrite_global_application_commands(client, app_id, &commands, NULL);
    discord_set_on_interaction_create(client, on_interaction);
}
