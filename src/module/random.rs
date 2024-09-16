use anyhow::{anyhow, Context};
use log::{debug, info};
use rand::{seq::SliceRandom, Rng};
use serde::Deserialize;
use serenity::{all::{CommandDataOptionValue, CommandInteraction, CommandOptionType, CreateCommand, CreateCommandOption, CreateInteractionResponseMessage, InstallationContext, InteractionContext}, async_trait};

use crate::Configuration;

use super::Module;

/// Struct to hold the coinflip command info
#[derive(Deserialize, Clone)]
pub struct Coinflip {
    /// Description of the coinflip command
    pub description: String,
    /// List of responses when the coin lands on heads
    pub heads_responses: Vec<String>,
    /// List of responses when the coin lands on tails
    pub tails_responses: Vec<String>
}

/// Struct to hold the random dice command info
#[derive(Deserialize, Clone)]
pub struct Dice {
    /// Description of the dice command
    pub description: String,
    /// List of responses when rolling any number
    pub default_responses: Vec<String>,
    /// 50/50 responses when rolling a one
    pub one_responses: Vec<String>,
    /// 50/50 responses when rolling a six
    pub six_responses: Vec<String>
}

/// Struct to hold the random number command info
#[derive(Deserialize, Clone)]
pub struct Number {
    /// Description of the number command
    pub description: String,
    /// List of responses when generating a number
    pub default_responses: Vec<String>,
}

/// Struct to hold the random command info
#[derive(Deserialize, Clone)]
pub struct Random {
    /// Coinflip command info
    pub coinflip: Coinflip,
    /// Dice command info
    pub dice: Dice,
    /// Number command info
    pub number: Number
}

/// Random module
pub struct RandomModule {
    /// Random command info
    random: Option<Random>
}

impl RandomModule {

    ///
    /// Create a new random module
    ///
    /// # Returns
    ///
    /// The random module
    ///
    pub fn new() -> Self {
        info!(target: "module/random", "creating random module");

        Self { random: None }
    }

}

#[async_trait]
impl Module for RandomModule {

    async fn init(&mut self, _: &serenity::all::Context, config: &Configuration) -> Result<Vec<CreateCommand>, anyhow::Error> {
        info!(target: "module/random", "initializing random module");
        self.random = Some(config.random.clone());
        let random = &self.random.as_ref().unwrap();

        // create commands
        info!(target: "module/random", "creating random command");
        Ok(vec![
            CreateCommand::new("random")
                .description("Let random numbers decide your fate")
                .integration_types(vec![InstallationContext::User, InstallationContext::Guild])
                .contexts(vec![InteractionContext::PrivateChannel, InteractionContext::Guild, InteractionContext::BotDm])
                .add_option(
                    CreateCommandOption::new(CommandOptionType::SubCommand, "coinflip", random.coinflip.description.clone())
                )
                .add_option(
                    CreateCommandOption::new(CommandOptionType::SubCommand, "dice", random.dice.description.clone())
                )
                .add_option(
                    CreateCommandOption::new(CommandOptionType::SubCommand, "number", random.number.description.clone())
                        .add_sub_option(CreateCommandOption::new(CommandOptionType::Integer, "min", "The minimum number to generate").required(true))
                        .add_sub_option(CreateCommandOption::new(CommandOptionType::Integer, "max", "The maximum number to generate").required(true))
                )
        ])
    }

    fn handles(&self, cmd: &CommandInteraction) -> bool {
        cmd.data.name == "random"
    }

    async fn handle(&mut self, ctx: serenity::all::Context, cmd: CommandInteraction) -> Result<(), anyhow::Error> {
        debug!(target: "module/random", "handling command {} executed by @{}", cmd.data.name, cmd.user.name);

        // get random command
        let random = self.random.as_ref().unwrap();
        let options = cmd.data.options.get(0)
            .context("no subcommand")?;

        // get response
        let response = match options.name.as_str() {
            "coinflip" => {
                if rand::random() {
                    random.coinflip.heads_responses.choose(&mut rand::thread_rng())
                } else {
                    random.coinflip.tails_responses.choose(&mut rand::thread_rng())
                }.context("no option")?.clone()
            },
            "dice" => {
                let roll = rand::thread_rng().gen_range(1..=6);
                match roll {
                    1 => {
                        if rand::random() {
                            random.dice.one_responses.choose(&mut rand::thread_rng())
                        } else {
                            random.dice.default_responses.choose(&mut rand::thread_rng())
                        }
                    },
                    6 => {
                        if rand::random() {
                            random.dice.six_responses.choose(&mut rand::thread_rng())
                        } else {
                            random.dice.default_responses.choose(&mut rand::thread_rng())
                        }
                    },
                    _ => random.dice.default_responses.choose(&mut rand::thread_rng())
                }.context("no option")?.clone().replace("{number}", roll.to_string().as_str())
            },
            "number" => {
                let options = match &options.value {
                    CommandDataOptionValue::SubCommand(o) => Some(o),
                    _ => None
                }.context("invalid subcommand")?;

                let mut min = options.get(0).and_then(|opt| opt.value.as_i64())
                    .context("no min")?;
                let mut max = options.get(1).and_then(|opt| opt.value.as_i64())
                    .context("no max")?;

                if min >= max {
                    let temp = min;
                    min = max;
                    max = temp;
                }

                let number = rand::thread_rng().gen_range(min..=max);
                random.number.default_responses.choose(&mut rand::thread_rng())
                    .context("no option")?
                    .replace("{number}", number.to_string().as_str())
            },
            _ => return Err(anyhow!("unknown subcommand"))
        };

        // send response
        cmd.create_response(&ctx.http, serenity::all::CreateInteractionResponse::Message(
            CreateInteractionResponseMessage::new()
                .content(response.replace("{user}", format!("<@{}>", cmd.user.id.get()).as_str()))
        )).await.context("failed to send response")?;

        Ok(())
    }

}