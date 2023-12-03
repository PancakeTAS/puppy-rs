package gay.pancake.purrify;

import gay.pancake.purrify.api.ApiEndpoints;
import gay.pancake.purrify.api.NekosBestApi;
import net.dv8tion.jda.api.JDA;
import net.dv8tion.jda.api.JDABuilder;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.hooks.ListenerAdapter;
import net.dv8tion.jda.api.interactions.commands.OptionType;
import net.dv8tion.jda.api.interactions.commands.build.OptionData;
import net.dv8tion.jda.api.requests.GatewayIntent;
import net.dv8tion.jda.api.utils.FileUpload;
import net.dv8tion.jda.internal.interactions.CommandDataImpl;
import org.jetbrains.annotations.NotNull;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.util.Arrays;
import java.util.concurrent.CompletableFuture;

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

    /**
     * Handle slash command interaction
     *
     * @param event Slash command interaction event
     */
    @Override
    public void onSlashCommandInteraction(@NotNull SlashCommandInteractionEvent event) {
        // Defer reply because we need to download the image
        event.deferReply().queue(defer -> {

            // Get endpoint
            var category = event.getFullCommandName();
            var endpoint = ApiEndpoints.getByName(category);

            // Parse response
            if (endpoint.getType() == ApiEndpoints.Type.IMAGE) {
                var amountOption = event.getOption("amount");
                var amount = amountOption == null ? 1 : Math.min(10, amountOption.getAsInt());

                // Request images
                this.api.getImage(category, amount).thenAccept(images -> {

                    // Combine futures
                    var futures = images.stream().map(image -> this.api.get(image.url)).toList();
                    CompletableFuture.allOf(futures.toArray(CompletableFuture[]::new)).thenAccept(v -> {

                        // Create reply message
                        var artists = new StringBuilder();
                        var sources = new StringBuilder();
                        var files = new FileUpload[images.size()];
                        for (var i = 0; i < images.size(); i++) {
                            artists.append(images.get(i).artist_name).append(": <").append(images.get(i).artist_href).append(">\n");
                            sources.append("<").append(images.get(i).source_url).append(">").append("\n");
                            files[i] = FileUpload.fromData(futures.get(i).join(), "image" + i + "." + endpoint.getType().getExtension());
                        }

                        // Reply with images
                        defer.sendMessage(endpoint.getMessage().replace("%SENDER%", event.getUser().getAsMention()) + "\n\nArtists:\n" + artists + "\nSources:\n" + sources)
                                .addFiles(files).queue(msg -> {
                                    // Close files
                                    for (var file: files) {
                                        try {
                                            file.close();
                                        } catch (IOException e) {
                                            LOGGER.warn("Failed to close file", e);
                                        }
                                    }
                                });
                    });

                });
            } else {
                var userOption = event.getOption("user");
                var user = userOption == null ? null : userOption.getAsUser();

                // Request gif
                this.api.getGif(category, 1).thenAccept(gifs -> {
                    var gif = gifs.getFirst();
                    // Get gif
                    this.api.get(gifs.getFirst().url).thenAccept(image -> {
                        // Reply with gif
                        var file = FileUpload.fromData(image, "image." + endpoint.getType().getExtension());
                        if (user == null) {
                            defer.sendMessage(endpoint.getMessage().replace("%SENDER%", event.getUser().getAsMention()) + "\n\n" + "Anime: " + gif.anime_name)
                                    .addFiles(file).queue(msg -> {
                                        // Close file
                                        try {
                                            file.close();
                                        } catch (IOException e) {
                                            LOGGER.warn("Failed to close file", e);
                                        }
                                    });
                        } else {
                            defer.sendMessage(endpoint.getWithUserMessage().replace("%SENDER%", event.getUser().getAsMention()).replace("%TARGET%", user.getAsMention()) + "\n\n" + "Anime: " + gif.anime_name)
                                    .addFiles(file).queue(msg -> {
                                        // Close file
                                        try {
                                            file.close();
                                        } catch (IOException e) {
                                            LOGGER.warn("Failed to close file", e);
                                        }
                                    });
                        }
                    });
                });
            }
        });
    }
}
