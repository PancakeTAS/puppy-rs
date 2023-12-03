package gay.pancake.purrify;

import gay.pancake.purrify.api.ApiEndpoints;
import gay.pancake.purrify.api.NekosBestApi;
import net.dv8tion.jda.api.JDA;
import net.dv8tion.jda.api.JDABuilder;
import net.dv8tion.jda.api.hooks.ListenerAdapter;
import net.dv8tion.jda.api.interactions.commands.OptionType;
import net.dv8tion.jda.api.interactions.commands.build.OptionData;
import net.dv8tion.jda.api.requests.GatewayIntent;
import net.dv8tion.jda.internal.interactions.CommandDataImpl;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Arrays;

/**
 * Purrify discord bot main class
 *
 * @author Pancake
 */
public class Purrify extends ListenerAdapter {
    public static void main(String[] args) throws Exception { new Purrify(); }

    /** Logger instance */
    public static final Logger LOGGER = LoggerFactory.getLogger("Purrify");

    /** JDA instance */
    private final JDA jda;

    /** nekos.best api */
    private final NekosBestApi api;

    /**
     * Initialize Purrify
     *
     * @throws Exception If something goes wrong
     */
    public Purrify() throws Exception {
        // Initialize JDA
        LOGGER.info("Initializing purrify discord bot...");
        var token = System.getenv("PURRIFY_TOKEN");
        this.jda = JDABuilder.createDefault(token)
                .enableIntents(GatewayIntent.MESSAGE_CONTENT)
                .addEventListeners(this)
                .build().awaitReady();

        // Initialize api
        LOGGER.info("Initializing nekos.best api...");
        this.api = new NekosBestApi();


        // Initialize commands
        LOGGER.info("Initializing commands...");
        var commands = Arrays.stream(ApiEndpoints.values()).map(e -> {
            var commandData = new CommandDataImpl(e.getName(), "Get an image of the category " + e.getName());
            if (e.getType() == ApiEndpoints.Type.IMAGE)
                commandData.addOptions(new OptionData(OptionType.INTEGER, "amount", "Amount of images (max 10)", false));
            else
                commandData.addOptions(new OptionData(OptionType.USER, "user", "Target of the command", !e.isOptional()));
            return commandData;
        }).toArray(CommandDataImpl[]::new);
        this.jda.getGuilds().forEach(guild -> guild.updateCommands().addCommands(commands).queue());

        LOGGER.info("Purrify is ready!");
    }

}
