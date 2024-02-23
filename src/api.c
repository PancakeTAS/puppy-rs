#define NEKOSBEST_IMPL
#include "api.h"

#define SET_PNGS(endpoint_name, endpoint_description, endpoint_message) \
    if (strcmp(endpoint->name, endpoint_name) == 0) { \
        info->type = PNGS; \
        info->description = endpoint_description; \
        info->message = endpoint_message; \
    }

#define SET_GIF_NO_TARGET(endpoint_name, endpoint_message) \
    if (strcmp(endpoint->name, endpoint_name) == 0) { \
        info->type = GIF_NO_TARGET; \
        info->message = endpoint_message; \
    }

#define SET_GIF_TARGET(endpoint_name, endpoint_message) \
    if (strcmp(endpoint->name, endpoint_name) == 0) { \
        info->type = GIF_TARGET; \
        info->message = endpoint_message; \
    }

int fetch_endpoints(struct discord *client, endpoint_list *list) {
    // fetch all endpoints
    nekos_endpoint_list endpoints;
    nekos_status status = nekos_endpoints(&endpoints);
    if (status != NEKOS_OK) {
        log_fatal("Failed to fetch endpoints");
        return 1;
    }

    // create commands
    list->len = 0;
    list->endpoints = (endpoint_info**) malloc(endpoints.len * sizeof(endpoint_info*));
    for (size_t i = 0; i < endpoints.len; i++) {
        nekos_endpoint *endpoint = endpoints.endpoints[i];

        // create endpoint info
        endpoint_info* info = (endpoint_info*) malloc(sizeof(endpoint_info));
        info->endpoint = endpoint;
        info->message = NULL;
        info->description = (char*) malloc(64);
        snprintf(info->description, 64, DEFAULT_DESCRIPTION, endpoint->name, endpoint->format == NEKOS_PNG ? "image" : "gif");

        // populate endpoint info
        SET_PNGS("husbando", "Get pictures of husbandos", "Here's husbandos for you %s!");
        SET_PNGS("kitsune", "Get pictures of foxgirls", "Here's foxgirls for you %s!");
        SET_PNGS("neko", "Get pictures of catgirls", "Here's catgirls for you %s!");
        SET_PNGS("waifu", "Get pictures of waifus", "Here's waifus for you %s!");
        SET_GIF_NO_TARGET("blush", "%1$s is blushing!");
        SET_GIF_NO_TARGET("bored", "%1$s is bored!");
        SET_GIF_NO_TARGET("cry", "%1$s is crying!");
        SET_GIF_NO_TARGET("dance", "%1$s is dancing!");
        SET_GIF_NO_TARGET("facepalm", "%1$s is facepalming!");
        SET_GIF_NO_TARGET("happy", "%1$s is happy!");
        SET_GIF_NO_TARGET("laugh", "%1$s is laughing!");
        SET_GIF_NO_TARGET("lurk", "%1$s is lurking!");
        SET_GIF_NO_TARGET("nod", "%1$s is nodding!");
        SET_GIF_NO_TARGET("nope", "%1$s is noping!");
        SET_GIF_NO_TARGET("pout", "%1$s is pouting!");
        SET_GIF_NO_TARGET("nom", "%1$s is nomming!");
        SET_GIF_NO_TARGET("shrug", "%1$s is shrugging!");
        SET_GIF_NO_TARGET("sleep", "%1$s is sleeping!");
        SET_GIF_NO_TARGET("smile", "%1$s is smiling!");
        SET_GIF_NO_TARGET("smug", "%1$s is smugging!");
        SET_GIF_NO_TARGET("stare", "%1$s is staring!");
        SET_GIF_NO_TARGET("think", "%1$s is thinking!");
        SET_GIF_NO_TARGET("thumbsup", "%1$s is giving a thumbs up!");
        SET_GIF_NO_TARGET("wave", "%1$s is waving!");
        SET_GIF_NO_TARGET("wink", "%1$s is winking!");
        SET_GIF_NO_TARGET("yawn", "%1$s is yawning!");
        SET_GIF_TARGET("bite", "%1$s bit %2$s!");
        SET_GIF_TARGET("baka", "%2$s BAKA! %1$s is calling you a baka!");
        SET_GIF_TARGET("cuddle", "%1$s cuddled with %2$s!");
        SET_GIF_TARGET("feed", "%1$s fed %2$s!");
        SET_GIF_TARGET("highfive", "%1$s highfived %2$s!");
        SET_GIF_TARGET("handhold", "%1$s is holding %2$s's hand!");
        SET_GIF_TARGET("handshake", "%1$s is shaking %2$s's hand!");
        SET_GIF_TARGET("hug", "%1$s hugged %2$s!");
        SET_GIF_TARGET("kick", "%1$s kicked %2$s!");
        SET_GIF_TARGET("kiss", "%1$s kissed %2$s!");
        SET_GIF_TARGET("pat", "%1$s patted %2$s!");
        SET_GIF_TARGET("peck", "%1$s pecked %2$s!");
        SET_GIF_TARGET("poke", "%1$s poked %2$s!");
        SET_GIF_TARGET("punch", "%1$s punched %2$s!");
        SET_GIF_TARGET("shoot", "%1$s shot %2$s!");
        SET_GIF_TARGET("slap", "%1$s slapped %2$s!");
        SET_GIF_TARGET("tickle", "%1$s tickled %2$s!");
        SET_GIF_TARGET("yeet", "%1$s yeeted %2$s!");

        // unknown endpoint
        if (!info->message) {
            log_warn("Unknown endpoint %s", info->endpoint->name);
            continue;
        }

        // add command
        list->endpoints[list->len] = info;
        list->len++;
    }

    return 0;
}
