use serenity::{all::{CommandInteraction, Context, CreateCommand}, async_trait};

use crate::Configuration;

pub mod reaction;
pub mod status;

/// The module trait.
#[async_trait]
pub trait Module: Send + Sync {

    ///
    /// Initialize all commands for the module
    ///
    /// # Arguments
    ///
    /// * `config` - The configuration
    ///
    /// # Returns
    ///
    /// A vector of command creation requests
    ///
    async fn init(&mut self, ctx: &Context, config: &Configuration) -> Result<Vec<CreateCommand>, anyhow::Error>;

    ///
    /// Check if the module handles the command
    ///
    /// # Arguments
    ///
    /// * `cmd` - The command interaction
    ///
    /// # Returns
    ///
    /// True if the module handles the command
    ///
    fn handles(&self, cmd: &CommandInteraction) -> bool;

    ///
    /// Handle the interaction
    ///
    /// # Arguments
    ///
    /// * `ctx` - The context
    /// * `cmd` - The command interaction
    ///
    async fn handle(&mut self, ctx: Context, cmd: CommandInteraction) -> Result<(), anyhow::Error>;

}
