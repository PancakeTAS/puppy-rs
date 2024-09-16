use anyhow::Context;
use log::{debug, info, trace};
use serde::Deserialize;
use serenity::{all::{CommandDataOptionValue, CommandInteraction, CommandOptionType, CreateCommand, CreateCommandOption, CreateEmbed, CreateInteractionResponseMessage, InstallationContext, InteractionContext, UserId}, async_trait};

use crate::Configuration;

use super::Module;

mod backend;

/// Struct to hold the reaction info
#[derive(Deserialize, Clone)]
pub struct Reaction {
    /// Name of the reaction
    pub name: String,
    /// Description of the reaction
    pub description: String,
    /// List of backends to fetch the reaction from
    pub backends: Vec<String>,
    /// List of responses when using the command as usual
    pub default_responses: Vec<String>,
    /// List of responses when using the command on the bot
    pub bot_responses: Vec<String>,
    /// List of responses when using the command on yourself
    pub self_responses: Vec<String>
}

/// Reaction module
pub struct ReactionModule {
    /// List of reactions
    reactions: Vec<Reaction>,
    /// Backend manager
    backend_manager: backend::BackendManager
}

impl ReactionModule {

    ///
    /// Create a new reaction module
    ///
    /// # Returns
    ///
    /// A Result containing the reaction module
    ///
    /// # Errors
    ///
    /// If the backend manager fails to initialize
    ///
    pub fn new() -> Result<Self, anyhow::Error> {
        info!(target: "module/reaction", "creating reaction module");
        let backend_manager = backend::BackendManager::new()?;

        Ok(Self {
            reactions: Vec::new(),
            backend_manager
        })
    }

}

#[async_trait]
impl Module for ReactionModule {

    async fn init(&mut self, _: &serenity::all::Context, config: &Configuration) -> Result<Vec<CreateCommand>, anyhow::Error> {
        info!(target: "module/reaction", "initializing reaction module");
        self.reactions = config.reactions.clone();

        // build cache
        self.backend_manager.build_cache().await?;

        // split reactions into commands of 25 options
        let mut index = 0;
        Ok(self.reactions.chunks(25).map(|batch| {
            // create command name
            index += 1;
            let index_str = index.to_string();
            let name = format!("reaction{}", if index > 1 { index_str.as_str() } else { "" });

            // create command
            info!(target: "module/reaction", "creating command '{}' with {} options", name, batch.len());
            CreateCommand::new(name)
                .description("React to someone with an animated gif.")
                .integration_types(vec![InstallationContext::User, InstallationContext::Guild])
                .contexts(vec![InteractionContext::PrivateChannel, InteractionContext::Guild, InteractionContext::BotDm])
                .set_options(
                    batch.iter().map(|i| {
                        CreateCommandOption::new(CommandOptionType::SubCommand, &i.name, &i.description)
                            .add_sub_option(CreateCommandOption::new(CommandOptionType::User, "user", "The target user.").required(true))
                    }).collect()
                )
        }).collect())
    }

    fn handles(&self, cmd: &CommandInteraction) -> bool {
        cmd.data.name.starts_with("reaction")
    }

    async fn handle(&mut self, ctx: serenity::all::Context, cmd: CommandInteraction) -> Result<(), anyhow::Error> {
        debug!(target: "module/reaction", "handling command {} executed by @{}", cmd.data.name, cmd.user.id);

        // get requested reaction
        let subcommand = cmd.data.options.get(0)
            .context("no subcommand")?;
        let options = match &subcommand.value {
            CommandDataOptionValue::SubCommand(o) => Some(o),
            _ => None
        }.context("invalid subcommand")?;
        let reaction = self.reactions.iter().find(|r| r.name == subcommand.name)
            .context("unknown reaction")?;

        // get user and target
        let user = cmd.user.id;
        let target = options.get(0).and_then(|opt| opt.value.as_user_id())
            .unwrap_or(UserId::new(cmd.application_id.get()));

        // pick random backend
        let backend_info = reaction.backends.get(rand::random::<usize>() % reaction.backends.len())
            .context("no backend")?;
        let mut backend_info = backend_info.splitn(2, "/");
        let (backend, endpoint): (&str, &str) = (backend_info.next().context("no backend")?, backend_info.next().context("no endpoint")?);

        info!(target: "module/reaction", "user @{} ran /reaction {} on <@{}>", cmd.user.name, reaction.name, target);

        // fetch reaction gif
        let image_url = self.backend_manager.get_cached(backend, endpoint)
            .context("no cached gif")?;

        // build response
        let message = if user == target {
            reaction.self_responses.get(rand::random::<usize>() % reaction.self_responses.len())
                .context("no self response")?
        } else if target == UserId::new(cmd.application_id.get()) {
            reaction.bot_responses.get(rand::random::<usize>() % reaction.bot_responses.len())
                .context("no bot response")?
        } else {
            reaction.default_responses.get(rand::random::<usize>() % reaction.default_responses.len())
                .context("no default response")?
        };

        let message = message.replace("{user}", format!("<@{}>", user.get()).as_str())
            .replace("{target}", format!("<@{}>", target.get()).as_str())
            + format!("\n-# From: {} • [Source](<{}>)", backend, image_url).as_str();

        let color = crate::color::rand();

        // send response
        trace!(target: "module/reaction", "sending response:\n{}\n{}", message, image_url);
        cmd.create_response(&ctx.http, serenity::all::CreateInteractionResponse::Message(
            CreateInteractionResponseMessage::new()
                .content(message)
                .embed(CreateEmbed::new()
                    .image(image_url)
                    .color(color)
                )
            )
        ).await.context("failed to send response")?;

        // refresh cache
        self.backend_manager.refresh_cache(backend, endpoint).await?;

        Ok(())
    }

}
