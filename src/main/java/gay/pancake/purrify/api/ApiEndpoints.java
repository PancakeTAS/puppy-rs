package gay.pancake.purrify.api;

import com.google.gson.annotations.SerializedName;
import lombok.AllArgsConstructor;
import lombok.Getter;

/**
 * Enum of api endpointsfor the nekos.best rest api
 *
 * @author Pancake
 */
@Getter @AllArgsConstructor
public enum ApiEndpoints {

    HUSBANDO(Type.IMAGE, "husbando", true, "Get a random husbando image", "Here's a husbando for you %SENDER%!", null),
    KITSUNE(Type.IMAGE, "kitsune", true, "Get a random kitsune image", "Here's a kitsune for you %SENDER%!", null),
    NEKO(Type.IMAGE, "neko", true, "Get a random neko image", "Here's a neko for you %SENDER%!", null),
    WAIFU(Type.IMAGE, "waifu", true, "Get a random waifu image", "Here's a waifu for you %SENDER%!", null),
    BAKA(Type.GIF, "baka", false, "Get a random baka gif", null, "%TARGET BAKA! %SENDER% is calling you a baka!"),
    BITE(Type.GIF, "bite", false, "Get a random bite gif", null, "%SENDER% bit %TARGET%!"),
    BLUSH(Type.GIF, "blush", true, "Get a random blush gif", "%SENDER% is blushing!", "%SENDER% is blushing at %TARGET%!"),
    BORED(Type.GIF, "bored", true, "Get a random bored gif", "%SENDER% is bored!", "%SENDER% is bored! %TARGET%!"),
    CRY(Type.GIF, "cry", true, "Get a random cry gif", "%SENDER% is crying!", "%SENDER% is crying! %TARGET%!"),
    CUDDLE(Type.GIF, "cuddle", false, "Get a random cuddle gif", null, "%SENDER% cuddled with %TARGET%!"),
    DANCE(Type.GIF, "dance", true, "Get a random dance gif", "%SENDER% is dancing!", "%SENDER% is dancing with %TARGET%!"),
    FACEPALM(Type.GIF, "facepalm", true, "Get a random facepalm gif", "%SENDER% is facepalming!", "%SENDER% is facepalming! %TARGET%!"),
    FEED(Type.GIF, "feed", false, "Get a random feed gif", null, "%SENDER% fed %TARGET%!"),
    HANDHOLD(Type.GIF, "handhold", false, "Get a random handhold gif", null, "%SENDER% is holding %TARGET%'s hand!"),
    HAPPY(Type.GIF, "happy", true, "Get a random happy gif", "%SENDER% is happy!", "%SENDER% is happy! %TARGET%!"),
    HIGHFIVE(Type.GIF, "highfive", false, "Get a random highfive gif", null, "%SENDER% highfived %TARGET%!"),
    HUG(Type.GIF, "hug", false, "Get a random hug gif", null, "%SENDER% hugged %TARGET%!"),
    KICK(Type.GIF, "kick", false, "Get a random kick gif", null, "%SENDER% kicked %TARGET%!"),
    KISS(Type.GIF, "kiss", false, "Get a random kiss gif", null, "%SENDER% kissed %TARGET%!"),
    LAUGH(Type.GIF, "laugh", true, "Get a random laugh gif", "%SENDER% is laughing!", "%SENDER% is laughing with %TARGET%!"),
    LURK(Type.GIF, "lurk", true, "Get a random lurk gif", "%SENDER% is lurking!", "%SENDER% is lurking! %TARGET%!"),
    NOD(Type.GIF, "nod", true, "Get a random nod gif", "%SENDER% is nodding!", "%SENDER% is nodding at %TARGET%!"),
    NOM(Type.GIF, "nom", true, "Get a random nom gif", "%SENDER% is nomming!", "%SENDER% is nomming %TARGET%!"),
    NOPE(Type.GIF, "nope", true, "Get a random nope gif", "%SENDER% is noping!", "%SENDER% is noping %TARGET%!"),
    PAT(Type.GIF, "pat", false, "Get a random pat gif", null, "%SENDER% patted %TARGET%!"),
    PECK(Type.GIF, "peck", false, "Get a random peck gif", null, "%SENDER% pecked %TARGET%!"),
    POKE(Type.GIF, "poke", false, "Get a random poke gif", null, "%SENDER% poked %TARGET%!"),
    POUT(Type.GIF, "pout", true, "Get a random pout gif", "%SENDER% is pouting!", "%SENDER% is pouting at %TARGET%!"),
    PUNCH(Type.GIF, "punch", false, "Get a random punch gif", null, "%SENDER% punched %TARGET%!"),
    SHOOT(Type.GIF, "shoot", false, "Get a random shoot gif", null, "%SENDER% shot %TARGET%!"),
    SHRUG(Type.GIF, "shrug", true, "Get a random shrug gif", "%SENDER% is shrugging!", "%SENDER% is shrugging at %TARGET%!"),
    SLAP(Type.GIF, "slap", false, "Get a random slap gif", null, "%SENDER% slapped %TARGET%!"),
    SLEEP(Type.GIF, "sleep", true, "Get a random sleep gif", "%SENDER% is sleeping!", "%SENDER% is sleeping with %TARGET%!"),
    SMILE(Type.GIF, "smile", true, "Get a random smile gif", "%SENDER% is smiling!", "%SENDER% is smiling at %TARGET%!"),
    SMUG(Type.GIF, "smug", true, "Get a random smug gif", "%SENDER% is smugging!", "%SENDER% is smugging at %TARGET%!"),
    STARE(Type.GIF, "stare", true, "Get a random stare gif", "%SENDER% is staring!", "%SENDER% is staring at %TARGET%!"),
    THINK(Type.GIF, "think", true, "Get a random think gif", "%SENDER% is thinking!", "%SENDER% is thinking about %TARGET%!"),
    THUMBSUP(Type.GIF, "thumbsup", true, "Get a random thumbsup gif", "%SENDER% is giving a thumbs up!", "%SENDER% is giving a thumbs up to %TARGET%!"),
    TICKLE(Type.GIF, "tickle", false, "Get a random tickle gif", null, "%SENDER% tickled %TARGET%!"),
    WAVE(Type.GIF, "wave", true, "Get a random wave gif", "%SENDER% is waving!", "%SENDER% is waving at %TARGET%!"),
    WINK(Type.GIF, "wink", true, "Get a random wink gif", "%SENDER% is winking!", "%SENDER% is winking at %TARGET%!"),
    YAWN(Type.GIF, "yawn", true, "Get a random yawn gif", "%SENDER% is yawning!", "%SENDER% is yawning at %TARGET%!"),
    YEET(Type.GIF, "yeet", false, "Get a random yeet gif", null, "%SENDER% yeeted %TARGET%!");

    /**
     * Type of image
     */
    @AllArgsConstructor @Getter
    public enum Type {

        /** PNG image */
        @SerializedName("png")
        IMAGE("png"),

        /** GIF image */
        @SerializedName("gif")
        GIF("gif");

        private final String extension;

    }

    /** Type of image */
    private final Type type;
    /** Name of category */
    private final String name;
    /** Whether the user is optional */
    private final boolean optional;
    /** Description of category */
    private final String description;
    /** Message to send when the user is not specified (%SENDER% is sender) */
    private final String message;
    /** Message to send when the user is specified (%SENDER% is sender and %TARGET% is target) */
    private final String withUserMessage;

    /**
     * Get endpoint by name
     *
     * @param name Name of endpoint
     * @return Endpoint or null if not found
     */
    public static ApiEndpoints getByName(String name) {
        for (var endpoint : values())
            if (endpoint.getName().equalsIgnoreCase(name))
                return endpoint;

        return null;
    }


}
