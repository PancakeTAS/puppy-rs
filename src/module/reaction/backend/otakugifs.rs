use anyhow::{anyhow, Context, Error};
use log::trace;
use serenity::async_trait;

use super::Backend;

/// The base URL for the otakugifs.xyz API
const API_URL: &str = "https://api.otakugifs.xyz/gif";

///
/// Simple and lightweight puppy-rs backend for fetching animated gifs off the otakugifs.xyz API
///
pub struct OtakuGifs;

#[async_trait]
impl Backend for OtakuGifs {

    fn id(&self) ->  &'static str {
        "otakugifs.xyz"
    }

    async fn endpoints(&self) -> Result<Vec<String>, Error> {
        // build request
        let url = format!("{}{}", API_URL, "/allreactions");

        // make request
        trace!(target: "module/reaction/backend/otakugifs", "fetching endpoints from '{}'", url);
        let response = reqwest::get(url).await
            .context("failed to make web request to api")?;
        let status = response.status();
        let body = response.text().await
            .context("failed to get response text")?;
        let mut body = body.as_str();

        // check error
        if !status.is_success() {
            return Err(anyhow!("otakugifs.xyz returned status code '{}':\n{}", status, body));
        }

        // parse response
        trace!(target: "module/reaction/backend/otakugifs", "parsing response:\n{}", body);
        let mut endpoints = Vec::new();
        body = body.split("[").last()
            .context("unable to find start of endpoints in response")?;
        while let Some(endname) = body.find("\",") {
            let startname = body[..endname].rfind("\"")
                .context("unable to find start of endpoint name in response")?;
            endpoints.push(body[startname+1..endname].to_string());
            body = &body[endname+1..];
        }
        if let Some(endname) = body.find("\"]") {
            let startname = body[..endname].rfind("\"")
                .context("unable to find start of endpoint name in response")?;
            endpoints.push(body[startname+1..endname].to_string());
        }

        // return endpoints
        trace!(target: "module/reaction/backend/otakugifs", "found endpoints: {:?}", endpoints);
        Ok(endpoints)
    }

    async fn fetch(&self, endpoint: &str) -> Result<String, Error> {
        // build request url
        let url = format!("{}{}{}", API_URL, "?reaction=", endpoint);

        // make request
        trace!(target: "module/reaction/backend/otakugifs", "fetching gif from '{}'", url);
        let response = reqwest::get(&url).await
            .context("failed to make web request to api")?;
        let status = response.status();
        let body = response.text().await
            .context("failed to get response text")?;

        // check error
        if !status.is_success() {
            return Err(anyhow!("otakugifs.xyz returned status code '{}':\n{}", status, body));
        }

        // parse response
        trace!(target: "module/reaction/backend/otakugifs", "parsing response:\n{}", body);
        let starturl = body.find("https://cdn.otakugifs.xyz/gifs/")
            .context("unable to find image url in response")?;
        let body = &body[starturl..];
        let endurl = body.find("\"")
            .context("unable to find end of image url in response")?;
        let body = &body[..endurl];

        // return url
        trace!(target: "module/reaction/backend/otakugifs", "found url: {}", body);
        Ok(body.to_string())
    }

}
