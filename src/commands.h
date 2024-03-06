#pragma once

#include "api.h"
#include "cache.h"

#include <concord/discord.h>
#include <concord/log.h>

/**
 * Prepare commands for the bot
 *
 * \param client The discord client
 * \param app_id The application id
 * \param endpoints The list of endpoints
 *
 * \return 0 on success, 1 on failure
 */
int prepare_commands(struct discord *client, u64snowflake app_id, endpoint_list *endpoints);
