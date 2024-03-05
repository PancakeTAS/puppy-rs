/**
 * \file
 * Main entry point for the bot
 */

#include <signal.h>
#include <stdlib.h>

#include "bot.h"

/**
 * Configuration file for the bot
 */
#define CONFIG_FILE "config.json"

/**
 * Discord client instance
 */
static struct discord *discord_client = NULL;

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
}

static int initialize() {
    // initialize concord
    CCORDcode code = ccord_global_init();
    if (code) {
        log_trace("[MAIN] ccord_global_init() failed: %d", code);
        return 1;
    }
    log_trace("[MAIN] ccord_global_init() success");

    // initialize discord client
    discord_client = discord_config_init(CONFIG_FILE);
    if (!discord_client) {
        log_trace("[MAIN] discord_config_init() failed");

        ccord_global_cleanup();
        return 1;
    }
    log_trace("[MAIN] discord_config_init() success");

    return 0;
}

/**
 * Main entry point for bot
 *
 * \return
 *   EXIT_SUCCESS Bot ran successfully \n
 *   EXIT_FAILURE Bot failed to run
 */
int main() {
    // initialize discord bot
    log_info("[MAIN] Initializing purrify discord bot...");
    if (initialize()) {
        log_fatal("[MAIN] Failed to initialize discord bot");

        return EXIT_FAILURE;
    }

    // run discord bot
    log_info("[MAIN] Launching discord bot...");
    signal(SIGINT, sigint_handler);
    discord_set_on_ready(discord_client, on_ready);
    CCORDcode code = discord_run(discord_client);

    // cleanup discord bot
    log_info("[MAIN] Discord bot exited (%d), cleanup up", code);
    discord_cleanup(discord_client);
    ccord_global_cleanup();
    deinit();
    return EXIT_SUCCESS;
}
