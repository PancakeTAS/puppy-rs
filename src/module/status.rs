use anyhow::anyhow;
use log::{debug, info};
use rand::seq::SliceRandom;
use serenity::{all::{ActivityData, CommandInteraction, CreateCommand, OnlineStatus}, async_trait};

use crate::Configuration;

use super::Module;

/// Status module
pub struct StatusModule {

}

impl StatusModule {

    ///
    /// Create a new status module
    ///
    /// # Returns
    ///
    /// The status module
    ///
    pub fn new() -> Self {
        info!(target: "module/status", "creating status module");

        Self {}
    }

}

#[async_trait]
impl Module for StatusModule {

    async fn init(&mut self, ctx: &serenity::all::Context, config: &Configuration) -> Result<Vec<CreateCommand>, anyhow::Error> {
        info!(target: "module/status", "initializing status module");
        let status_messages = config.status_messages.clone();

        // schedule timer to update status
        let ctx = ctx.clone();
        tokio::spawn(async move {
            loop {
                let status = status_messages.choose(&mut rand::thread_rng()).unwrap();

                debug!(target: "module/status", "setting status to: {}", status);
                ctx.set_presence(Some(ActivityData::custom(status)), OnlineStatus::Online);

                tokio::time::sleep(tokio::time::Duration::from_secs(60 * 5)).await;
            }
        });

        Ok(vec![])
    }

    fn handles(&self, _: &CommandInteraction) -> bool {
        false
    }

    async fn handle(&mut self, _: serenity::all::Context, _: CommandInteraction) -> Result<(), anyhow::Error> {
        Err(anyhow!("not implemented"))
    }

}