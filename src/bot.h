/**
 * \file
 * This file contains the bots main behavior.
 */

#pragma once

#include <concord/discord.h>
#include <concord/log.h>

#include "api.h"
#include "cache.h"
#include "commands.h"

/**
 * Initialize the discord bot
 *
 * \param client Discord client
 * \param event Ready event
 */
void init(struct discord *client, const struct discord_ready *event);

/**
 * Deinitialize the discord bot
 */
void deinit();
