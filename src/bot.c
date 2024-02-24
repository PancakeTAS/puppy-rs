#include "bot.h"

endpoint_list all_endpoints;

void init(struct discord *client, u64snowflake app_id) {
    // fetch endpoints
    log_info("[BOT] Initializing nekos.best API...");
    if (fetch_endpoints(&all_endpoints)) {
        discord_shutdown(client);
        return;
    }

    // prepare commands
    log_info("[BOT] Initializing slash commands...");
    prepare_commands(client, app_id, &all_endpoints);
}

void deinit() {
    // free endpoints
    for (int i = 0; i < all_endpoints.len; i++) {
        free(all_endpoints.endpoints[i]->endpoint->name);
        free(all_endpoints.endpoints[i]->endpoint);
        free(all_endpoints.endpoints[i]->description);
        free(all_endpoints.endpoints[i]);
    }
    free(all_endpoints.endpoints);
}