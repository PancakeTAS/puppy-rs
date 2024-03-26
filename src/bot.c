#include "bot.h"

endpoint_list all_endpoints;

/**
 * Update bot status
 *
 * \param client Discord client
 * \param event Ready event
 *
 * \return 0 on success, 1 on failure
 */
int update_status(struct discord *client, const struct discord_ready *event) {
    struct discord_presence_update presence = {
        .activities = &(struct discord_activities) {
            .size = 1,
            .array = &(struct discord_activity) {
                .name = "purrify",
                .state = malloc(48),
                .type = DISCORD_ACTIVITY_CUSTOM
            }
        },
        .status = "idle",
        .afk = false,
        .since = discord_timestamp(client)
    };
    snprintf(presence.activities->array->state, 48, "Serving %d guilds with %d endpoints.", event->guilds->size, all_endpoints.len);
    discord_set_presence(client, &presence);
    free(presence.activities->array->state);

    return 0;
}

void init(struct discord *client, const struct discord_ready *event) {
    // fetch endpoints
    if (fetch_endpoints(&all_endpoints)) {
        log_fatal("[BOT] Failed to fetch nekos.best API endpoints");

        discord_shutdown(client);
        return;
    }
    log_info("[BOT] Fetched nekos.best API endpoints");

    // ensure cache validity
    if (ensure_cache_validity(&all_endpoints)) {
        log_fatal("[BOT] Failed to ensure cache validity");

        discord_shutdown(client);
        return;
    }
    log_info("[BOT] Ensured cache validity");

    // prepare commands
    log_info("[BOT] Initializing slash commands...");
    if (prepare_commands(client, event->application->id, &all_endpoints)) {
        log_fatal("[BOT] Failed to initialize slash commands");

        discord_shutdown(client);
        return;
    }
    log_info("[BOT] Initialized slash commands");

    // update status
    log_info("[BOT] Updating status...");
    if (update_status(client, event)) {
        log_fatal("[BOT] Failed to update status");

        discord_shutdown(client);
        return;
    }
    log_info("[MAIN] Initialized status");
}

void deinit(void) {
    free_endpoints(&all_endpoints);
}
