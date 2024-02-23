/**
 * \file
 * This file contains the bots main behavior. 
 */

#pragma once

#include <concord/discord.h>
#include <concord/log.h>
#include "api.h"
#include "commands.h"

/**
 * Initialize the discord bot
 * 
 * \param client Discord client
 * \param app_id Application ID
 */
void init(struct discord *client, u64snowflake app_id);