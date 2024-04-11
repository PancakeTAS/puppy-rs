/// \file main.c Main entry point for the bot

#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include "api.h"
#include "cache.h"
#include "commands.h"
#include "log.h"

/// Configuration file for the bot
#define CONFIG_FILE "config.json"

bool is_initialized = false; ///< Whether the bot has been initialized before
endpoint_list all_endpoints; ///< List of all bot endpoints

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
 * \param client
 *   Discord client
 * \param event
 *   Timer event
 */
void update_status(struct discord *client, struct discord_timer *event) {
    // get guild count
    struct discord_ret_guilds ret = { .done = do_update_status };
    discord_get_current_user_guilds(client, &ret);
}

/**
 * Launch the bot. Called when bot is ready.
 *
 * \param client
 *   Discord client
 * \param event
 *   Ready event
 */
void on_ready(struct discord *client, const struct discord_ready *event) {
    if (is_initialized) return;
    is_initialized = true;

    // fetch endpoints
    if (fetch_endpoints(&all_endpoints)) {
        log_fatal("MAIN", "Failed to fetch nekos.best API endpoints");

        ccord_shutdown_async();
        return;
    }
    log_info("MAIN", "Fetched nekos.best endpoints");

    // ensure cache validity
    if (ensure_cache_validity(&all_endpoints)) {
        log_fatal("MAIN", "Failed to ensure cache validity");

        ccord_shutdown_async();
        return;
    }
    log_info("MAIN", "Ensured cache validity");

    // prepare commands
    log_info("MAIN", "Initializing slash commands...");
    if (prepare_commands(client, event->application->id, &all_endpoints)) {
        log_fatal("MAIN", "Failed to initialize slash commands");

        ccord_shutdown_async();
        return;
    }
    log_info("MAIN", "Initialized slash commands");

    // update status
    log_info("MAIN", "Initializing status update timer...");
    discord_timer_interval(client, update_status, NULL, NULL, 0, 60 * 60 * 1000, -1);
    log_info("MAIN", "Initialized status update timer");
}

/**
 * Handle SIGINT signal and shut down bot
 *
 * \param signum Signal number
 */
void sigint_handler(int signum) {
    log_info("MAIN", "Received SIGINT, shutting down");
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
        log_trace("MAIN", "ccord_global_init() failed: %d", code);

        return NULL;
    }

    // create discord client
    struct discord* client = discord_config_init(CONFIG_FILE);
    if (!client) {
        log_trace("MAIN", "discord_create() failed");

        ccord_global_cleanup();
        return NULL;
    }

    // update log level based on config
    char level[10];
    struct ccord_szbuf_readonly log_level = discord_config_get_field(client, (char*[2]) {
        "logging", "purrify_level"
    }, 2);
    snprintf(level, sizeof(level), "%.*s", (int) log_level.size, log_level.start);
    level[sizeof(level) - 1] = '\0';
    if (strcmp(level, "trace") == 0) {
        log_set_level(LOG_TRACE);
    } else if (strcmp(level, "debug") == 0) {
        log_set_level(LOG_DEBUG);
    } else if (strcmp(level, "info") == 0) {
        log_set_level(LOG_INFO);
    } else if (strcmp(level, "warn") == 0) {
        log_set_level(LOG_WARN);
    } else if (strcmp(level, "error") == 0) {
        log_set_level(LOG_ERROR);
    } else if (strcmp(level, "fatal") == 0) {
        log_set_level(LOG_FATAL);
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
    log_info("MAIN", "Initializing discord bot...");
    struct discord* client = initialize_discord();
    if (!client) {
        log_fatal("MAIN", "Failed to initialize discord bot");

        return EXIT_FAILURE;
    }

    // run discord bot
    log_info("MAIN", "Launching discord bot...");
    signal(SIGINT, sigint_handler);
    discord_set_on_ready(client, on_ready);
    CCORDcode code = discord_run(client);

    // cleanup discord bot
    log_info("MAIN", "Discord bot exited (%d), cleanup up", code);
    free_endpoints(&all_endpoints);
    discord_cleanup(client);
    ccord_global_cleanup();
    return EXIT_SUCCESS;
}
