#pragma once

#include <concord/discord.h>
#include <concord/log.h>

#include "api.h"

/**
 * Prepare commands for the bot
 * 
 * \param client The discord client
 * \param app_id The application id
 * \param endpoints The list of endpoints 
 */
void prepare_commands(struct discord *client, u64snowflake app_id, endpoint_list *endpoints);
