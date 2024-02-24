#define NEKOSBEST_IMPL
#include "api.h"

#define SET_PNG(endpoint_name, endpoint_description, endpoint_message) \
    if (strcmp(endpoint->name, endpoint_name) == 0) { \
        info->type = PNG; \
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
        SET_PNG("husbando", "Get pictures of husbandos", "Here's husbando for you <@%1$ld>!");
        SET_PNG("kitsune", "Get pictures of foxgirls", "Here's foxgirls for you <@%1$ld>!");
        SET_PNG("neko", "Get pictures of catgirls", "Here's catgirls for you <@%1$ld>!");
        SET_PNG("waifu", "Get pictures of waifus", "Here's waifus for you <@%1$ld>!");
        SET_GIF_NO_TARGET("blush", "<@%1$ld> is blushing!");
        SET_GIF_NO_TARGET("bored", "<@%1$ld> is bored!");
        SET_GIF_NO_TARGET("cry", "<@%1$ld> is crying!");
        SET_GIF_NO_TARGET("dance", "<@%1$ld> is dancing!");
        SET_GIF_NO_TARGET("facepalm", "<@%1$ld> is facepalming!");
        SET_GIF_NO_TARGET("happy", "<@%1$ld> is happy!");
        SET_GIF_NO_TARGET("laugh", "<@%1$ld> is laughing!");
        SET_GIF_NO_TARGET("lurk", "<@%1$ld> is lurking!");
        SET_GIF_NO_TARGET("nod", "<@%1$ld> is nodding!");
        SET_GIF_NO_TARGET("nope", "<@%1$ld> is noping!");
        SET_GIF_NO_TARGET("pout", "<@%1$ld> is pouting!");
        SET_GIF_NO_TARGET("nom", "<@%1$ld> is nomming!");
        SET_GIF_NO_TARGET("shrug", "<@%1$ld> is shrugging!");
        SET_GIF_NO_TARGET("sleep", "<@%1$ld> is sleeping!");
        SET_GIF_NO_TARGET("smile", "<@%1$ld> is smiling!");
        SET_GIF_NO_TARGET("smug", "<@%1$ld> is smugging!");
        SET_GIF_NO_TARGET("stare", "<@%1$ld> is staring!");
        SET_GIF_NO_TARGET("think", "<@%1$ld> is thinking!");
        SET_GIF_NO_TARGET("thumbsup", "<@%1$ld> is giving a thumbs up!");
        SET_GIF_NO_TARGET("wave", "<@%1$ld> is waving!");
        SET_GIF_NO_TARGET("wink", "<@%1$ld> is winking!");
        SET_GIF_NO_TARGET("yawn", "<@%1$ld> is yawning!");
        SET_GIF_TARGET("bite", "<@%1$ld> bit <@%2$ld>!");
        SET_GIF_TARGET("baka", "<@%2$ld> BAKA! <@%1$ld> is calling you a baka!");
        SET_GIF_TARGET("cuddle", "<@%1$ld> cuddled with <@%2$ld>!");
        SET_GIF_TARGET("feed", "<@%1$ld> fed <@%2$ld>!");
        SET_GIF_TARGET("highfive", "<@%1$ld> highfived <@%2$ld>!");
        SET_GIF_TARGET("handhold", "<@%1$ld> is holding <@%2$ld>'s hand!");
        SET_GIF_TARGET("handshake", "<@%1$ld> is shaking <@%2$ld>'s hand!");
        SET_GIF_TARGET("hug", "<@%1$ld> hugged <@%2$ld>!");
        SET_GIF_TARGET("kick", "<@%1$ld> kicked <@%2$ld>!");
        SET_GIF_TARGET("kiss", "<@%1$ld> kissed <@%2$ld>!");
        SET_GIF_TARGET("pat", "<@%1$ld> patted <@%2$ld>!");
        SET_GIF_TARGET("peck", "<@%1$ld> pecked <@%2$ld>!");
        SET_GIF_TARGET("poke", "<@%1$ld> poked <@%2$ld>!");
        SET_GIF_TARGET("punch", "<@%1$ld> punched <@%2$ld>!");
        SET_GIF_TARGET("shoot", "<@%1$ld> shot <@%2$ld>!");
        SET_GIF_TARGET("slap", "<@%1$ld> slapped <@%2$ld>!");
        SET_GIF_TARGET("tickle", "<@%1$ld> tickled <@%2$ld>!");
        SET_GIF_TARGET("yeet", "<@%1$ld> yeeted <@%2$ld>!");

        // unknown endpoint
        if (!info->message) {
            log_warn("Unknown endpoint %ld", info->endpoint->name);
            continue;
        }

        // add command
        list->endpoints[list->len] = info;
        list->len++;
    }

    return 0;
}

void download_picture(endpoint_result *result, endpoint_info *endpoint) {
    // fetch pictures
    nekos_result_list api_results;
    nekos_category(&api_results, endpoint->endpoint, 1);

    // download files
    result->results = (nekos_http_response*) malloc(sizeof(nekos_http_response));
    result->infos = api_results.responses[0];
    nekos_download(&result->results[0], api_results.responses[0]->url);
}
