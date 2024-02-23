#include "bot.h"

void init(struct discord *client, u64snowflake app_id) {
    // fetch endpoints
    log_info("Fetching endpoints...");
    endpoint_list endpoints;
    if (fetch_endpoints(client, &endpoints)) {
        log_fatal("Failed to fetch endpoints");
        discord_shutdown(client);
        return;
    }
    log_info("Fetched all available endpoints");

    // prepare commands
    log_info("Preparing commands...");
    prepare_commands(client, app_id, &endpoints);
    log_info("Commands prepared");

}