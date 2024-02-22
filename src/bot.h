/**
 * \file
 * This file contains the bots main behavior. 
 */

#pragma once

#include <concord/discord.h>
typedef struct discord discord;
#include <concord/log.h>
#include "api.h"

/**
 * Initialize the discord bot 
 */
void init(discord *client);