use std::collections::HashMap;

use anyhow::{anyhow, Context};
use log::{info, trace};
use serenity::async_trait;

mod nekosbest;
mod otakugifs;

/// The backend trait.
#[async_trait]
trait Backend: Send + Sync {

    ///
    /// Get the ID of the backend.
    ///
    /// # Returns
    ///
    /// The ID of the backend
    ///
    fn id(&self) -> &'static str;

    ///
    /// Fetch the list of supported endpoints
    ///
    /// # Returns
    ///
    /// A Result containing a list of supported endpoints
    ///
    /// # Errors
    ///
    /// If the request fails or the response is not successful
    ///
    async fn endpoints(&self) -> Result<Vec<String>, anyhow::Error>;

    ///
    /// Fetch a random animated gif from the otakugifs.xyz API
    ///
    /// # Arguments
    ///
    /// * `endpoint` - The endpoint to fetch from
    ///
    /// # Returns
    ///
    /// A Result containing the url of the animated gif
    ///
    /// # Errors
    ///
    /// If the request fails or the response is not successful
    ///
    async fn fetch(&self, endpoint: &str) -> Result<String, anyhow::Error>;

}

///
/// Backend manager for the '/reaction' command module.
///
/// Provides a way to manage multiple backends as well as cache their endpoints.
///
pub struct BackendManager {
    backends: HashMap<&'static str, Box<dyn Backend>>,
    caches: HashMap<&'static str, HashMap<String, String>>,
}

impl BackendManager {

    ///
    /// Create a new backend manager.
    ///
    /// # Returns
    ///
    /// A new backend manager.
    ///
    pub fn new() -> Result<Self, anyhow::Error> {
        let mut backends: HashMap<&'static str, Box<dyn Backend>> = HashMap::new();
        let mut caches = HashMap::new();

        // add backends
        let nekosbest = nekosbest::NekosBest;
        let id = nekosbest.id();
        backends.insert(id, Box::new(nekosbest));
        caches.insert(id, HashMap::new());
        info!(target: "module/reaction/backend", "registered nekos.best backend");

        let otakugifs = otakugifs::OtakuGifs;
        let id = otakugifs.id();
        backends.insert(id, Box::new(otakugifs));
        caches.insert(id, HashMap::new());
        info!(target: "module/reaction/backend", "registered otakugifs.xyz backend");

        // build manager
        Ok(Self {
            backends,
            caches
        })
    }

    ///
    /// Build the initial cache of endpoints for all backends.
    ///
    /// # Errors
    ///
    /// Returns an error if the cache build fails.
    ///
    pub async fn build_cache(&mut self) -> Result<(), anyhow::Error> {
        info!(target: "module/reaction/backend", "building cache, this may take a while...");

        for backend in self.backends.values() {
            let id = backend.id();

            // fetch list of endpoints
            let endpoints = backend.endpoints().await
                .context("failed to fetch endpoints")?;
            let endpoints_len = endpoints.len();

            // fetch a gif from each endpoint
            for endpoint in endpoints {
                let gif = backend.fetch(&endpoint).await
                    .context("failed to fetch gif")?;

                // cache the gif
                self.caches.get_mut(id).unwrap().insert(endpoint, gif);

                // wait 200ms before fetching the next gif
                tokio::time::sleep(std::time::Duration::from_millis(200)).await;
            }

            info!(target: "module/reaction/backend", "cached {} endpoints for backend '{}'", endpoints_len, id);
        }

        info!(target: "module/reaction/backend", "cache build complete");
        Ok(())
    }

    ///
    /// Get a cached gif from the specified backend.
    ///
    /// # Arguments
    ///
    /// * `backend` - The backend to fetch the gif from.
    /// * `endpoint` - The endpoint to fetch the gif from.
    ///
    /// # Returns
    ///
    /// A string containing the URL of the fetched gif.
    ///
    /// # Errors
    ///
    /// Returns an error if the backend is unknown or if the gif is not cached.
    ///
    pub fn get_cached(&mut self, backend: &str, endpoint: &str) -> Result<String, anyhow::Error> {
        trace!(target: "module/reaction/backend", "requested '{}' endpoint on cached backend '{}'", endpoint, backend);

        // get backend and cache
        let backend = self.backends.get(backend)
            .ok_or_else(|| anyhow!("unknown backend: {}", backend))?;
        let cache = self.caches.get_mut(backend.id())
            .ok_or_else(|| anyhow!("no cache for backend: {}", backend.id()))?;

        // try to fetch the gif from the cache
        let url = cache.remove(endpoint)
            .ok_or_else(|| anyhow!("unknown endpoint: {}", endpoint))?;

        // return the url
        Ok(url)
    }

    ///
    /// Refresh the cache for the specified backend.
    ///
    /// # Arguments
    ///
    /// * `backend` - The backend to refresh the cache for.
    /// * `endpoint` - The endpoint to refresh the cache for.
    ///
    /// # Errors
    ///
    /// Returns an error if the backend is unknown or if the cache refresh fails.
    ///
    pub async fn refresh_cache(&mut self, backend: &str, endpoint: &str) -> Result<(), anyhow::Error> {
        trace!(target: "module/reaction/backend", "refreshing '{}' endpoint on cached backend '{}'", endpoint, backend);

        // get backend and cache
        let backend = self.backends.get(backend)
            .ok_or_else(|| anyhow!("unknown backend: {}", backend))?;
        let cache = self.caches.get_mut(backend.id())
            .ok_or_else(|| anyhow!("no cache for backend: {}", backend.id()))?;

        // fetch the gif
        let gif = backend.fetch(endpoint).await
            .context("failed to fetch gif")?;

        // cache the gif
        cache.insert(endpoint.to_string(), gif);

        Ok(())
    }

}
