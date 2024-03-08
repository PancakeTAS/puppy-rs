
#define NEKOSBEST_IMPL
#include <nekosbest.h>

#include <concord/discord.h>
#include <concord/log.h>
#include <string.h>
#include <errno.h>

#include "api.h"

#define SET_PNG(endpoint_name, endpoint_description, endpoint_message) \
    if (strcmp(bot_endpoint->name, endpoint_name) == 0) { \
        bot_endpoint->type = PNG; \
        strcpy(bot_endpoint->description, endpoint_description); \
        strcpy(bot_endpoint->message, endpoint_message); \
    }

#define SET_GIF_NO_TARGET(endpoint_name, endpoint_message) \
    if (strcmp(bot_endpoint->name, endpoint_name) == 0) { \
        bot_endpoint->type = GIF_NO_TARGET; \
        strcpy(bot_endpoint->message, endpoint_message); \
    }

#define SET_GIF_TARGET(endpoint_name, endpoint_message) \
    if (strcmp(bot_endpoint->name, endpoint_name) == 0) { \
        bot_endpoint->type = GIF_TARGET; \
        strcpy(bot_endpoint->message, endpoint_message); \
    }

int fetch_endpoints(endpoint_list *bot_endpoint_list) {
    // fetch all endpoints
    nekos_endpoint_list api_endpoint_list;
    nekos_status api_status = nekos_endpoints(&api_endpoint_list);
    if (api_status) {
        log_trace("[NEKOS_API] nekos_endpoints() failed: %d", api_status);

        return 1;
    }
    log_trace("[NEKOS_API] nekos_endpoints() success: %d endpoints", api_endpoint_list.len);

    // create commands
    bot_endpoint_list->len = 0;
    bot_endpoint_list->endpoints = (endpoint_info**) calloc(api_endpoint_list.len, sizeof(endpoint_info*));
    if (!bot_endpoint_list->endpoints) {
        log_trace("[NEKOS_API] malloc() failed: %s", strerror(errno));

        nekos_free_endpoints(&api_endpoint_list);
        return 1;
    }
    log_trace("[NEKOS_API] malloc() success: %p", bot_endpoint_list->endpoints);

    for (size_t i = 0; i < api_endpoint_list.len; i++) {
        nekos_endpoint *api_endpoint = &api_endpoint_list.endpoints[i];

        // create endpoint info
        endpoint_info* bot_endpoint = (endpoint_info*) calloc(1, sizeof(endpoint_info));
        if (!bot_endpoint) {
            log_trace("[NEKOS_API] malloc() failed: %s", strerror(errno));

            nekos_free_endpoints(&api_endpoint_list);
            free_endpoints(bot_endpoint_list);
            return 1;
        }
        log_trace("[NEKOS_API] malloc() success: %p", bot_endpoint);

        bot_endpoint->name = strdup(api_endpoint->name);
        bot_endpoint->format = api_endpoint->format;

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
        SET_GIF_NO_TARGET("sleep", "<@%1$ld> is eepy!");
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
        if (!bot_endpoint->message[0]) {
            log_warn("[NEKOS_API] Unknown endpoint %s", bot_endpoint->name);
            continue;
        }

        // set default description
        if (!bot_endpoint->description[0])
            snprintf(bot_endpoint->description, 64, DEFAULT_DESCRIPTION, bot_endpoint->name, bot_endpoint->format == NEKOS_PNG ? "image" : "gif");

        // add command
        log_debug("[NEKOS_API] Adding endpoint %s", bot_endpoint->name);
        bot_endpoint_list->endpoints[bot_endpoint_list->len] = bot_endpoint;
        bot_endpoint_list->len++;
    }

    // free memory
    nekos_free_endpoints(&api_endpoint_list);

    return 0;
}

int download_picture(endpoint_result *bot_result, endpoint_info *bot_endpoint) {
    // fetch api
    nekos_result_list api_results;
    nekos_status status = nekos_category(&api_results, &(nekos_endpoint) { .name = bot_endpoint->name, .format = bot_endpoint->format }, 1);
    if (status) {
        log_trace("[NEKOS_API] nekos_category() failed: %d", status);

        return 1;
    }
    log_trace("[NEKOS_API] nekos_category() success: %d result(s)", api_results.len);

    // download picture
    nekos_http_response api_response;
    nekos_result* api_result = &api_results.responses[0];
    status = nekos_download(&api_response, api_result->url);
    if (status) {
        log_trace("[NEKOS_API] nekos_download() failed: %d", status);

        nekos_free_results(&api_results, bot_endpoint->format);
        return status;
    }
    log_trace("[NEKOS_API] nekos_download() success: %d bytes", api_response.len);

    strcpy(bot_result->message, bot_endpoint->message);
    int base = strlen(bot_result->message);
    if (bot_endpoint->type == PNG)
        snprintf(bot_result->message + base, 2000 - base, "\n\nArtist:\n%s: <%s>\n\nSource:\n<%s>", api_result->source.png->artist_name, api_result->source.png->artist_href, api_result->source.png->source_url);
    else
        snprintf(bot_result->message + base, 2000 - base, "\n\nAnime: %s", api_result->source.gif->anime_name);

    // create response file
    bot_result->file = (char*) malloc(api_response.len);
    if (!bot_result->file) {
        log_trace("[NEKOS_API] malloc() failed: %s", strerror(errno));

        nekos_free_results(&api_results, bot_endpoint->format);
        nekos_free_http_response(&api_response);
        return 1;
    }
    log_trace("[NEKOS_API] malloc() success: %p", bot_result->file);

    memcpy(bot_result->file, api_response.text, api_response.len);
    bot_result->file_len = api_response.len;

    // free memory
    nekos_free_results(&api_results, bot_endpoint->format);
    nekos_free_http_response(&api_response);

    return 0;
}

void free_endpoints(endpoint_list *list) {
    for (int i = 0; i < list->len; i++) {
        free(list->endpoints[i]->name);
        free(list->endpoints[i]);
    }
    free(list->endpoints);
}

void free_result(endpoint_result *result) {
    free(result->file);
}
