
#define NEKOSBEST_IMPL
#include <nekosbest.h>
#define OTAKUGIFS_IMPL
#include <otakugifs.h>

#include <concord/discord.h>
#include <string.h>
#include <errno.h>

#include "api.h"
#include "log.h"

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

    // fetch all endpoints from api
    nekos_endpoint_list api_endpoint_list;
    nekos_status api_status = nekos_endpoints(&api_endpoint_list);
    if (api_status) {
        log_fatal("API", "nekos_endpoints() failed: %d", api_status);
        return 1;
    }
    log_trace("API", "nekos_endpoints() success: Fetched %lu endpoints", api_endpoint_list.len);

    otaku_reaction_list otaku_endpoint_list;
    otaku_status otaku_status = otaku_reactions(&otaku_endpoint_list);
    if (otaku_status) {
        log_fatal("API", "otaku_reactions() failed: %d", otaku_status);
        return 1;
    }
    log_trace("API", "otaku_reactions() success: Fetched %lu endpoints", otaku_endpoint_list.len);

    // allocate memory for bot endpoints
    bot_endpoint_list->len = 0;
    bot_endpoint_list->endpoints = (endpoint_info*) calloc(api_endpoint_list.len + otaku_endpoint_list.len, sizeof(endpoint_info));
    if (!bot_endpoint_list->endpoints) {
        log_fatal("API", "calloc() failed: %s", strerror(errno));
        return 1;
    }
    log_trace("API", "calloc() success: Allocating %lu endpoints", api_endpoint_list.len);


    // populate bot endpoints from nekos.best
    for (size_t i = 0; i < api_endpoint_list.len; i++) {
        nekos_endpoint *api_endpoint = &api_endpoint_list.endpoints[i];

        // create endpoint info
        endpoint_info* bot_endpoint = &bot_endpoint_list->endpoints[bot_endpoint_list->len];
        bot_endpoint->format = api_endpoint->format;
        bot_endpoint->api = 1;
        bot_endpoint->name = strdup(api_endpoint->name);
        if (!bot_endpoint->name) {
            log_fatal("API", "strdup() failed: %s", strerror(errno));
            return 1;
        }
        log_trace("API", "strdup() success: Created bot endpoint base with name %s and format %d", bot_endpoint->name, bot_endpoint->format);

        // populate endpoint info
        SET_PNG("husbando", "Get pictures of husbandos", "Here's husbando for you <@%1$ld>!");
        SET_PNG("kitsune", "Get pictures of foxgirls", "Here's foxgirls for you <@%1$ld>!");
        SET_PNG("neko", "Get pictures of catgirls", "Here's catgirls for you <@%1$ld>!");
        SET_PNG("waifu", "Get pictures of waifus", "Here's waifus for you <@%1$ld>!");
        SET_GIF_NO_TARGET("blush", "<@%1$ld> is blushing!");
        SET_GIF_NO_TARGET("cry", "<@%1$ld> is crying!");
        SET_GIF_NO_TARGET("dance", "<@%1$ld> is dancing!");
        SET_GIF_NO_TARGET("happy", "<@%1$ld> is happy!");
        SET_GIF_NO_TARGET("laugh", "<@%1$ld> is laughing!");
        SET_GIF_NO_TARGET("lurk", "<@%1$ld> is lurking!");
        SET_GIF_NO_TARGET("pout", "<@%1$ld> is pouting!");
        SET_GIF_NO_TARGET("nom", "<@%1$ld> is nomming!");
        SET_GIF_NO_TARGET("sleep", "<@%1$ld> is eepy!");
        SET_GIF_NO_TARGET("smile", "<@%1$ld> is smiling!");
        SET_GIF_NO_TARGET("smug", "<@%1$ld> is smugging!");
        SET_GIF_NO_TARGET("stare", "<@%1$ld> is staring!");
        SET_GIF_NO_TARGET("think", "<@%1$ld> is thinking!");
        SET_GIF_NO_TARGET("thumbsup", "<@%1$ld> is giving a thumbs up!");
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
            log_info("API", "Endpoint '%s' from nekos.best is not supported", bot_endpoint->name);

            free(bot_endpoint->name);
            continue;
        }

        // no description
        if (!bot_endpoint->description[0]) {
            log_trace("API", "Setting default description for endpoint %s", bot_endpoint->name);
            snprintf(bot_endpoint->description, 64, DEFAULT_DESCRIPTION, bot_endpoint->name, api_endpoint->format == NEKOS_PNG ? "image" : "gif");
        }

        // add command
        log_debug("API", "Initialized endpoint %s with description %s", bot_endpoint->name, bot_endpoint->description);
        bot_endpoint_list->len++;

    }

    // append bot endpoints from otakugifs.xyz
    for (size_t i = 0; i < otaku_endpoint_list.len; i++) {
        char* otaku_endpoint = otaku_endpoint_list.reactions[i];

        endpoint_info* bot_endpoint = &bot_endpoint_list->endpoints[bot_endpoint_list->len];

        // check if endpoint already exists
        bool is_duplicate = false;
        for (int j = 0; j < bot_endpoint_list->len; j++) {
            if (strcmp(otaku_endpoint, bot_endpoint_list->endpoints[j].name) == 0) {
                bot_endpoint_list->endpoints[j].api = 3;
                is_duplicate = true;
                break;
            }
        }

        if (is_duplicate)
            continue;

        // create endpoint info
        bot_endpoint->format = NEKOS_GIF;
        bot_endpoint->api = 2;
        bot_endpoint->name = strdup(otaku_endpoint);
        if (!bot_endpoint->name) {
            log_fatal("API", "strdup() failed: %s", strerror(errno));
            return 1;
        }
        log_trace("API", "strdup() success: Created bot endpoint base with name %s and format gif", bot_endpoint->name);

        // populate endpoint info
        SET_GIF_NO_TARGET("confused", "<@%1$ld> is confused!");
        SET_GIF_NO_TARGET("drool", "<@%1$ld> is drooling!");
        SET_GIF_NO_TARGET("evillaugh", "<@%1$ld> is laughing evilly!");
        SET_GIF_NO_TARGET("love", "<@%1$ld> is in love!");
        SET_GIF_NO_TARGET("mad", "<@%1$ld> is mad!");
        SET_GIF_NO_TARGET("nervous", "<@%1$ld> is nervous!");
        SET_GIF_NO_TARGET("nyah", "<@%1$ld> nya!");
        SET_GIF_NO_TARGET("sad", "<@%1$ld> is sad!");
        SET_GIF_NO_TARGET("scared", "<@%1$ld> is scared!");
        SET_GIF_NO_TARGET("shy", "<@%1$ld> is shy!");
        SET_GIF_NO_TARGET("surprised", "<@%1$ld> is surprised!");
        SET_GIF_TARGET("lick", "<@%1$ld> licked <@%2$ld>!");
        SET_GIF_TARGET("bite", "<@%1$ld> bit <@%2$ld>!");
        SET_GIF_TARGET("cuddle", "<@%1$ld> cuddled with <@%2$ld>!");
        SET_GIF_TARGET("handhold", "<@%1$ld> is holding <@%2$ld>'s hand!");
        SET_GIF_TARGET("hug", "<@%1$ld> hugged <@%2$ld>!");
        SET_GIF_TARGET("kiss", "<@%1$ld> kissed <@%2$ld>!");
        SET_GIF_TARGET("pat", "<@%1$ld> patted <@%2$ld>!");
        SET_GIF_TARGET("poke", "<@%1$ld> poked <@%2$ld>!");
        SET_GIF_TARGET("punch", "<@%1$ld> punched <@%2$ld>!");
        SET_GIF_TARGET("slap", "<@%1$ld> slapped <@%2$ld>!");
        SET_GIF_TARGET("tickle", "<@%1$ld> tickled <@%2$ld>!");
        SET_GIF_TARGET("angrystare", "<@%1$ld> is angrily staring at <@%2$ld>!");
        SET_GIF_TARGET("nuzzle", "<@%1$ld> is nuzzling <@%2$ld>!");
        SET_GIF_TARGET("peek", "<@%1$ld> is peeking at <@%2$ld>!");
        SET_GIF_TARGET("pinch", "<@%1$ld> pinched <@%2$ld>!");

        // unknown endpoint
        if (!bot_endpoint->message[0]) {
            log_info("API", "Endpoint '%s' from otakugifs.xyz is not supported", bot_endpoint->name);

            free(bot_endpoint->name);
            continue;
        }

        // no description
        if (!bot_endpoint->description[0]) {
            log_trace("API", "Setting default description for endpoint %s", bot_endpoint->name);
            snprintf(bot_endpoint->description, 64, DEFAULT_DESCRIPTION, bot_endpoint->name, "gif");
        }

        // add command
        log_debug("API", "Initialized endpoint %s with description %s", bot_endpoint->name, bot_endpoint->description);
        bot_endpoint_list->len++;

    }

    // free memory
    nekos_free_endpoints(&api_endpoint_list);
    otaku_free_reactions(&otaku_endpoint_list);

    log_debug("API", "Successfully initialized %d endpoints", bot_endpoint_list->len);
    srand(time(NULL));

    return 0;
}

int download_picture(endpoint_result *bot_result, endpoint_info *bot_endpoint) {
    nekos_http_response api_response;

    //bool use_otaku_api = bot_endpoint->api == 3 ? !(rand() % 4) : (bot_endpoint->api & 2);
    bool use_nekosbest = bot_endpoint->api & 1;
    if (bot_endpoint->api == 3) { // 75% chance to use new otakus api
        use_nekosbest = (rand() % 4) == 0;
    }

    if (use_nekosbest) {
        nekos_result_list api_results = { 0 };

        // fetch api
        nekos_status status = nekos_category(&api_results, &(nekos_endpoint) { .name = bot_endpoint->name, .format = bot_endpoint->format }, 1);
        if (status) {
            log_error("API", "nekos_category() failed: %d", status);
            return 1;
        }
        log_trace("API", "nekos_category() success: Fetched %lu results", api_results.len);


        // download picture
        nekos_result* api_result = &api_results.responses[0];
        status = nekos_download(&api_response, api_result->url);
        if (status) {
            log_error("API", "nekos_download() failed: %d", status);

            nekos_free_results(&api_results);
            return status;
        }
        log_trace("API", "nekos_download() success: Downloaded %lu bytes", api_response.len);

        // free memory
        nekos_free_results(&api_results);
    } else {
        otaku_result otaku_result;

        // fetch api
        otaku_status status = otaku_reaction(&otaku_result, bot_endpoint->name, OTAKU_GIF);
        if (status) {
            log_error("API", "otaku_reaction() failed: %d", status);
            return 1;
        }
        log_trace("API", "otaku_reaction() success: Fetched result");

        // download picture
        status = otaku_download((otaku_http_response*) &api_response, otaku_result.url);
        if (status) {
            log_error("API", "otaku_download() failed: %d", status);

            otaku_free_result(&otaku_result);
            return status;
        }

        // free memory
        otaku_free_result(&otaku_result);
    }


    // populate bot result with formatted message
    strcpy(bot_result->message, bot_endpoint->message);

    // create response file
    bot_result->file = (char*) malloc(api_response.len);
    if (!bot_result->file) {
        log_error("API", "malloc() failed: %s", strerror(errno));

        nekos_free_http_response(&api_response);
        return 1;
    }
    memcpy(bot_result->file, api_response.text, api_response.len);
    bot_result->file_len = api_response.len;
    log_trace("API", "malloc() success: Copied response file with %lu bytes", bot_result->file_len);


    // free memory
    nekos_free_http_response(&api_response);

    log_debug("API", "Successfully downloaded picture from endpoint %s", bot_endpoint->name);

    return 0;
}

void free_endpoints(endpoint_list *list) {
    for (int i = 0; i < list->len; i++)
        free(list->endpoints[i].name);
    free(list->endpoints);
}

void free_result(endpoint_result *result) {
    free(result->file);
}
