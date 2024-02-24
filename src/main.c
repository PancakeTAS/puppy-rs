/**
 * \file
 * Main entry point for the bot
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "bot.h"

/**
 * Configuration file for the bot
 */
#define CONFIG_FILE "config.json"


/**
 * Discord client instance
 */
static struct discord *discord_client;

/**
 * Launch the bot. Called when bot is ready.
 * 
 * \param client Discord client
 * \param event Ready event
 */
void on_ready(struct discord *client, const struct discord_ready *event) {
    init(client, event->application->id);
}

/**
 * Handle SIGINT signal and shut down bot
 * 
 * \param signum Signal number 
 */
void sigint_handler(int signum) {
    log_info("[MAIN] Received SIGINT, shutting down");
    discord_shutdown(discord_client);
    signal(SIGINT, sigint_handler);
}

/**
 * Main entry point for bot
 * 
 * \return
 *   EXIT_SUCCESS Bot ran successfully \n
 *   EXIT_FAILURE Bot failed to run
 */
int main() {
    signal(SIGINT, sigint_handler);

    // initialize concord
    CCORDcode code = ccord_global_init();
    if (code != CCORD_OK) {
        log_fatal("[MAIN] Failed to initialize global resources");
        return EXIT_FAILURE;
    }

    // initialize discord client
    struct discord *client = discord_client = discord_config_init(CONFIG_FILE);
    if (!client) {
        log_fatal("[MAIN] Failed to initialize discord client");
        return EXIT_FAILURE;
    }

    // start discord client
    log_info("[MAIN] Starting discord client");
    discord_set_on_ready(client, on_ready);
    code = discord_run(client);
    if (code != CCORD_OK) {
        log_fatal("[MAIN] Failed to start discord client");
        discord_cleanup(client);
        ccord_global_cleanup();
        return EXIT_FAILURE;
    }

    // cleanup discord client
    log_info("[MAIN] Closing discord client");
    deinit();
    discord_cleanup(client);
    ccord_global_cleanup();
    return EXIT_SUCCESS;
}
