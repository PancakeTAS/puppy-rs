#include "bot.h"

#define SET_PNG(endpoint_name, description, message) \
    if (strcmp(cmd->endpoint->name, endpoint_name) == 0) { \
        cmd->command_description = description; \
        cmd->response_message = message; \
    }

#define SET_GIF(endpoint_name, message) \
    if (strcmp(cmd->endpoint->name, endpoint_name) == 0) { \
        cmd->response_message = message; \
    }

#define SET_GIF_TARGET(endpoint_name, message) \
    if (strcmp(cmd->endpoint->name, endpoint_name) == 0) { \
        cmd->response_message_user = message; \
    }

static void new_command(command_data* cmd) {
    // pngs with amount and custom message
    SET_PNG("husbando", "Get pictures of husbandos", "Here's husbandos for you %s!");
    SET_PNG("kitsune", "Get pictures of foxgirls", "Here's foxgirls for you %s!");
    SET_PNG("neko", "Get pictures of catgirls", "Here's catgirls for you %s!");
    SET_PNG("waifu", "Get pictures of waifus", "Here's waifus for you %s!");
    // gifs without target user
    SET_GIF("blush", "%1$s is blushing!");
    SET_GIF("bored", "%1$s is bored!");
    SET_GIF("cry", "%1$s is crying!");
    SET_GIF("dance", "%1$s is dancing!");
    SET_GIF("facepalm", "%1$s is facepalming!");
    SET_GIF("happy", "%1$s is happy!");
    SET_GIF("laugh", "%1$s is laughing!");
    SET_GIF("lurk", "%1$s is lurking!");
    SET_GIF("nod", "%1$s is nodding!");
    SET_GIF("nope", "%1$s is noping!");
    SET_GIF("pout", "%1$s is pouting!");
    SET_GIF("nom", "%1$s is nomming!");
    SET_GIF("shrug", "%1$s is shrugging!");
    SET_GIF("sleep", "%1$s is sleeping!");
    SET_GIF("smile", "%1$s is smiling!");
    SET_GIF("smug", "%1$s is smugging!");
    SET_GIF("stare", "%1$s is staring!");
    SET_GIF("think", "%1$s is thinking!");
    SET_GIF("thumbsup", "%1$s is giving a thumbs up!");
    SET_GIF("wave", "%1$s is waving!");
    SET_GIF("wink", "%1$s is winking!");
    SET_GIF("yawn", "%1$s is yawning!");
    // gifs with target user
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
    if (cmd->response_message == NULL && cmd->response_message_user == NULL) {
        log_warn("Unknown endpoint %s", cmd->endpoint->name);
        return;
    }

    // register command (print for now)
    if (cmd->response_message_user != NULL) {
        log_info(cmd->response_message_user, "me", "you");
    } else {
        log_info(cmd->response_message, "me");
    }

}

void init(discord *client) {
    // fetch endpoints
    log_info("Fetching endpoints...");
    if (fetch_endpoints(&new_command)) {
        log_fatal("Failed to fetch endpoints");
        discord_shutdown(client);
        return;
    }
    log_info("Fetched all available endpoints");
}