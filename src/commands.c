#include "commands.h"

void prepare_commands(struct discord *client, u64snowflake app_id, endpoint_list *endpoints) {
    // create command options
    struct discord_application_command_options amount_options = {
        .size = 1,
        .array = &(struct discord_application_command_option) {
            .type = DISCORD_APPLICATION_OPTION_INTEGER,
            .name = "amount",
            .description = "Amount of images to fetch",
            .required = false,
            .min_value = "1",
            .max_value = "10"
        }
    };

    struct discord_application_command_options user_options = {
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
    for (size_t i = 0; i < endpoints->len; i++) {
        endpoint_info *info = endpoints->endpoints[i];

        command_params[i].default_permission = true;
        command_params[i].application_id = app_id;
        command_params[i].dm_permission = false;
        command_params[i].type = DISCORD_APPLICATION_CHAT_INPUT;
        command_params[i].description = info->description;
        command_params[i].name = info->endpoint->name;

        if (info->type == PNGS)
            command_params[i].options = &amount_options;
        else if (info->type == GIF_NO_TARGET)
            command_params[i].options = &user_options;
        else
            command_params[i].options = NULL;

        log_info("Created command %s", command_params[i].name);
    }

    // create commands
    struct discord_application_commands commands = {
        .array = command_params,
        .size = endpoints->len
    };
    discord_bulk_overwrite_global_application_commands(client, app_id, &commands, NULL);
}