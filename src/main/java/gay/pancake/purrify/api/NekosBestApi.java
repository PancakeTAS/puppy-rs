package gay.pancake.purrify.api;

import com.google.gson.Gson;
import com.google.gson.annotations.SerializedName;
import com.google.gson.reflect.TypeToken;
import lombok.Data;
import lombok.Getter;

import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.util.List;
import java.util.Map;
import java.util.concurrent.CompletableFuture;

/**
 * Interface for the nekos.best rest api
 *
 * @author Pancake
 */
@Getter
public class NekosBestApi {

    /**
     * Result of an image request
     */
    @Data
    public static class ImageResult {

        /** Artist link */
        @SerializedName("artist_href")
        public final String artist_href;

        /** Artist name */
        @SerializedName("artist_name")
        public final String artist_name;

        /** Original image url */
        @SerializedName("source_url")
        public final String source_url;

        /** Image url */
        @SerializedName("url")
        public final String url;

    }

    /**
     * Result of an gif request
     */
    @Data
    public static class GifResult {

        /** Artist link */
        @SerializedName("anime_name")
        public final String anime_name;

        /** Image url */
        @SerializedName("url")
        public final String url;

    }

    /** Base url of api */
    private static final String URL = "https://nekos.best/api/v2/";
    /** Http client instance */
    private static final HttpClient CLIENT = HttpClient.newHttpClient();
    /** Gson instance */
    private static final Gson GSON = new Gson();

    /**
     * Gets a random image or GIF from the available categories along with its metadata.
     * The amount query may be used to retrieve multiple assets at once. The amount is a number such that 1 <= X <= 20.
     *
     * @param category Category to get image from
     * @param amount Amount of images to get (1 <= x <= 20)
     * @return List of image results
     */
    public CompletableFuture<List<ImageResult>> getImage(String category, int amount) {
        if (amount < 1 || amount > 20)
            throw new IllegalArgumentException("Amount must be between 1 and 20");

        return CLIENT.sendAsync(
                HttpRequest.newBuilder(URI.create(URL + category + "?amount=" + amount)).GET().build(),
                HttpResponse.BodyHandlers.ofString()
        ).thenApply(s ->
                ((Map<String, List<ImageResult>>) GSON.fromJson(s.body(), new TypeToken<Map<String, List<ImageResult>>>(){}.getType())).get("results")
        );
    }

    /**
     * Gets a random image or GIF from the available categories along with its metadata.
     * The amount query may be used to retrieve multiple assets at once. The amount is a number such that 1 <= X <= 20.
     *
     * @param category Category to get image from
     * @param amount Amount of images to get (1 <= x <= 20)
     * @return List of gif results
     */
    public CompletableFuture<List<GifResult>> getGif(String category, int amount) {
        if (amount < 1 || amount > 20)
            throw new IllegalArgumentException("Amount must be between 1 and 20");

        return CLIENT.sendAsync(
                HttpRequest.newBuilder(URI.create(URL + category + "?amount=" + amount)).GET().build(),
                HttpResponse.BodyHandlers.ofString()
        ).thenApply(s ->
                ((Map<String, List<GifResult>>) GSON.fromJson(s.body(), new TypeToken<Map<String, List<GifResult>>>(){}.getType())).get("results")
        );
    }

    /**
     * Gets a specific image from our categories. Replace :filename with the asset's filename and :format with the category's format.
     * Note: The asset's metadata are provided URL-encoded, in the response's headers under anime_name, artist_name, artist_href and source_url.
     *
     * @param url Url to get image from
     * @return Image bytes
     */
    public CompletableFuture<byte[]> get(String url) {
        return CLIENT.sendAsync(
                HttpRequest.newBuilder(URI.create(url)).GET().build(),
                HttpResponse.BodyHandlers.ofByteArray()
        ).thenApply(HttpResponse::body);
    }

}
