use anyhow::{anyhow, Context, Error};
use log::trace;
use reqwest::header::USER_AGENT;
use serenity::async_trait;

use crate::BOT_USER_AGENT;

use super::Backend;

/// The base URL for the nekos.best API
const API_URL: &str = "https://nekos.best/api/v2/";

///
/// Simple and lightweight puppy-rs backend for fetching animated gifs off the nekos.best API
///
pub struct NekosBest {
    http: reqwest::Client
}

impl NekosBest {
    pub fn new() -> Self {
        Self { http: reqwest::Client::new() }
    }
}

#[async_trait]
impl Backend for NekosBest {

    fn id(&self) ->  &'static str {
        "nekos.best"
    }

    async fn endpoints(&self) -> Result<Vec<String>, Error> {
        // build request
        let url = format!("{}{}", API_URL, "endpoints");

        // make request
        trace!(target: "module/reaction/backend/nekosbest", "fetching endpoints from '{}'", url);
        let response = self.http.get(url).header(USER_AGENT, BOT_USER_AGENT).send().await
            .context("failed to make web request to api")?;
        let status = response.status();
        let body = response.text().await
            .context("failed to get response text")?;
        let mut body = body.as_str();

        // check error
        if !status.is_success() {
            return Err(anyhow!("nekos.best returned status code '{}':\n{}", status, body));
        }

        // parse response
        trace!(target: "module/reaction/backend/nekosbest", "parsing response:\n{}", body);
        let mut endpoints = Vec::new();
        body = body.split("png\"},").last()
            .context("unable to find start of gif endpoints in response")?;
        while let Some(endname) = body.find("\":{") {
            let startname = body[..endname].rfind("\"")
                .context("unable to find start of endpoint name in response")?;
            endpoints.push(body[startname+1..endname].to_string());
            body = &body[endname+1..];
        }

        // return endpoints
        trace!(target: "module/reaction/backend/nekosbest", "found endpoints: {:?}", endpoints);
        Ok(endpoints)
    }

    async fn fetch(&self, endpoint: &str) -> Result<String, Error> {
        // build request url
        let url = format!("{}{}", API_URL, endpoint);

        // make request
        trace!(target: "module/reaction/backend/nekosbest", "fetching gif from '{}'", url);
        let response = self.http.get(url).header(USER_AGENT, BOT_USER_AGENT).send().await
            .context("failed to make web request to api")?;
        let status = response.status();
        let body = response.text().await
            .context("failed to get response text")?;

        // check error
        if !status.is_success() {
            return Err(anyhow!("nekos.best returned status code '{}':\n{}", status, body));
        }

        // parse response
        trace!(target: "module/reaction/backend/nekosbest", "parsing response:\n{}", body);
        let starturl = body.find("https://nekos.best/api")
            .context("unable to find image url in response")?;
        let body = &body[starturl..];
        let endurl = body.find("\"")
            .context("unable to find end of image url in response")?;
        let body = &body[..endurl];

        // return url
        trace!(target: "module/reaction/backend/nekosbest", "found url: {}", body);
        Ok(body.to_string())
    }

}
