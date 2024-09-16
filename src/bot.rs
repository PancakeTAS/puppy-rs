
use std::sync::Arc;

use log::{info, warn};
use serenity::{all::{Command, CommandInteraction, EventHandler, GatewayIntents, Interaction}, async_trait, Client};
use tokio::sync::RwLock;

use crate::{module::{reaction::ReactionModule, Module}, Configuration};

///
/// The bot struct
///
struct Bot {
    /// The configuration
    config: Arc<RwLock<Configuration>>,
    /// The modules
    modules: Arc<RwLock<Vec<Box<dyn Module>>>>
}

impl Bot {

    ///
    /// Try to handle a command with a module
    ///
    /// # Arguments
    ///
    /// * `ctx` - The context
    /// * `command` - The command interaction
    ///
    async fn try_handle_command(&self, ctx: serenity::client::Context, command: CommandInteraction) {
        let mut modules = self.modules.write().await;

        // find module that handles command
        let module = modules.iter_mut().find(|module| module.handles(&command));

        // handle command
        if let Some(module) = module {
            match module.handle(ctx, command).await {
                Ok(_) => {},
                Err(e) => warn!(target: "bot", "failed to handle command: {}", e)
            };
        } else {
            warn!(target: "bot", "no module handles command: {}", command.data.name);
        }
    }

}

#[async_trait]
impl EventHandler for Bot {

    async fn ready(&self, ctx: serenity::client::Context, ready: serenity::model::gateway::Ready) {
        let mut modules = self.modules.write().await;
        let config = self.config.write().await;

        // initialize modules
        let mut commands = vec![];
        for module in modules.iter_mut() {
            commands.extend(
                module.init(&config).await
                .expect("failed to initialize module")
            );
        }

        // set commands
        Command::set_global_commands(&ctx.http, commands).await
        .expect("failed to set commands");

        info!(target: "bot", "bot is ready: {}", ready.user.name);
    }


    async fn interaction_create(&self, ctx: serenity::all::Context, interaction: Interaction) {
        if let Interaction::Command(command) = interaction {
            self.try_handle_command(ctx, command).await;
        }
    }

}

///
/// Launch the bot
///
/// # Arguments
///
/// * `config` - The configuration
///
/// # Returns
///
/// Ok if the bot ran successfully
///
/// # Errors
///
/// An error if the bot failed to launch
///
pub async fn launch_bot(config: Configuration) -> Result<(), anyhow::Error> {
    info!(target: "bot", "launching bot");
    let token = config.discord_token.clone();

    // create modules
    info!(target: "bot", "creating modules");
    let mut modules: Vec<Box<dyn Module>> = vec![];

    info!(target: "bot", "creating reaction module");
    let reaction_module = Box::new(ReactionModule::new()?);
    modules.push(reaction_module);

    let bot = Bot {
        config: Arc::new(RwLock::new(config)),
        modules: Arc::new(RwLock::new(modules))
    };

    // launch bot
    info!(target: "bot", "connecting to discord");
    Client::builder(token, GatewayIntents::empty())
        .event_handler(bot)
        .await
        .expect("client failed to build")
        .start()
        .await
        .expect("client failed to start");

    Ok(())
}