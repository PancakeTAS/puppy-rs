/// \file commands.h This file prepares commands for the bot

#pragma once

#include "api.h"

#include <concord/discord.h>

/**
 * Prepare commands for the bot
 *
 * \param client
 *   The discord client
 * \param app_id
 *   The application id
 * \param endpoints
 *   The list of endpoints
 *
 * \return
 *   0 on success, 1 on fatal failure. The application is expected to exit on failure, as no cleanup is performed.
 */
int prepare_commands(struct discord *client, u64snowflake app_id, endpoint_list *endpoints);
