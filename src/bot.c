#include "bot.h"
#include "log.h"
#include "api.h"
#include "cache.h"
#include "commands.h"

endpoint_list all_endpoints;

void do_update_status(struct discord *client, struct discord_response *response, const struct discord_guilds *guilds) {
    // update status
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
    snprintf(presence.activities->array->state, 48, "Serving %d guilds with %d endpoints.", guilds->size, all_endpoints.len);
    discord_set_presence(client, &presence);
    free(presence.activities->array->state);
}

/**
 * Update bot status
 *
 * \param client Discord client
 * \param event Ready event
 */
void update_status(struct discord *client, struct discord_timer *event) {
    // get guild count
    struct discord_ret_guilds ret = { .done = do_update_status };
    discord_get_current_user_guilds(client, &ret);
}

void init(struct discord *client, const struct discord_ready *event) {
    // fetch endpoints
    if (fetch_endpoints(&all_endpoints)) {
        log_fatal("BOT", "Failed to fetch nekos.best API endpoints");

        discord_shutdown(client);
        return;
    }
    log_info("BOT", "Fetched nekos.best API endpoints");

    // ensure cache validity
    if (ensure_cache_validity(&all_endpoints)) {
        log_fatal("BOT", "Failed to ensure cache validity");

        discord_shutdown(client);
        return;
    }
    log_info("BOT", "Ensured cache validity");

    // prepare commands
    log_info("BOT", "Initializing slash commands...");
    if (prepare_commands(client, event->application->id, &all_endpoints)) {
        log_fatal("BOT", "Failed to initialize slash commands");

        discord_shutdown(client);
        return;
    }
    log_info("BOT", "Initialized slash commands");

    // update status
    log_info("BOT", "Initializing status update timer...");
    discord_timer_interval(client, update_status, NULL, NULL, 0, 60 * 60 * 1000, -1);
    log_info("MAIN", "Initialized status update timer");

}

void deinit() {
    free_endpoints(&all_endpoints);
}
