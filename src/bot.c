#include "bot.h"

endpoint_list all_endpoints;

void init(struct discord *client, u64snowflake app_id) {
    // fetch endpoints
    log_info("Fetching endpoints...");
    if (fetch_endpoints(&all_endpoints)) {
        log_fatal("Failed to fetch endpoints");
        discord_shutdown(client);
        return;
    }
    log_info("Fetched all available endpoints");

    // prepare commands
    log_info("Preparing commands...");
    prepare_commands(client, app_id, &all_endpoints);
    log_info("Commands prepared");
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