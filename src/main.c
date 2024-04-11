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

bool is_initialized = false; ///< Whether the bot has been initialized

/**
 * Launch the bot. Called when bot is ready.
 *
 * \param client Discord client
 * \param event Ready event
 */
void on_ready(struct discord *client, const struct discord_ready *event) {
    if (is_initialized) return;
    is_initialized = true;

    init(client, event);
}

/**
 * Handle SIGINT signal and shut down bot
 *
 * \param signum Signal number
 */
void sigint_handler(int signum) {
    log_info("[MAIN] Received SIGINT, shutting down");
    ccord_shutdown_async();
}

/**
 * Initialize discord client
 *
 * \return Discord client on success, NULL on failure
 */
static struct discord* initialize_discord() {
    // initialize concord
    CCORDcode code = ccord_global_init();
    if (code) {
        log_trace("[MAIN] ccord_global_init() failed: %d", code);

        return NULL;
    }

    // create discord client
    struct discord* client = discord_config_init(CONFIG_FILE);
    if (!client) {
        log_trace("[MAIN] discord_create() failed");

        ccord_global_cleanup();
        return NULL;
    }

    return client;
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
    log_info("[MAIN] Initializing discord bot...");
    struct discord* client = initialize_discord();
    if (!client) {
        log_fatal("[MAIN] Failed to initialize discord bot");

        return EXIT_FAILURE;
    }

    // run discord bot
    log_info("[MAIN] Launching discord bot...");
    signal(SIGINT, sigint_handler);
    discord_set_on_ready(client, on_ready);
    CCORDcode code = discord_run(client);

    // cleanup discord bot
    log_info("[MAIN] Discord bot exited (%d), cleanup up", code);
    discord_cleanup(client);
    ccord_global_cleanup();
    deinit();
    return EXIT_SUCCESS;
}
