#include "bot.h"

endpoint_list all_endpoints;

void init(struct discord *client, u64snowflake app_id) {
    // fetch endpoints
    if (fetch_endpoints(&all_endpoints)) {
        log_fatal("[BOT] Failed to fetch nekos.best API endpoints");

        discord_shutdown(client);
        return;
    }
    log_info("[BOT] Fetched nekos.best API endpoints");

    // prepare commands
    log_info("[BOT] Initializing slash commands...");
    if (prepare_commands(client, app_id, &all_endpoints)) {
        log_fatal("[BOT] Failed to initialize slash commands");

        discord_shutdown(client);
        return;
    }
    log_info("[BOT] Initialized slash commands");
}

void deinit() {
    free_endpoints(&all_endpoints);
}