use std::{fs::File, path::PathBuf, str::FromStr};

use anyhow::Context;
use clap::{Arg, ArgAction, Command};
use colog::format::CologStyle;
use colored::Colorize;
use log::{Level, LevelFilter};
use module::{random::Random, reaction::Reaction};
use serde::Deserialize;

pub mod bot;
pub mod color;
pub mod module;

/// Configuration file structure
#[derive(Deserialize)]
pub struct Configuration {
    pub status_messages: Vec<String>,
    pub random: Random,
    pub reactions: Vec<Reaction>,
    pub discord_token: String,
    pub log_level: String
}

pub struct CustomPrefixToken;

impl CologStyle for CustomPrefixToken {
    fn prefix_token(&self, level: &Level) -> String {
        format!(
            "[{}] [{}]",
            chrono::Local::now().format("%d.%m.%Y %H:%M:%S"),
            match level {
                Level::Error => "E".red(),
                Level::Warn => "W".yellow(),
                Level::Info => "*".green(),
                Level::Debug => "D".blue(),
                Level::Trace => "T".purple(),
            }
        )
    }
}

#[tokio::main]
async fn main() -> Result<(), anyhow::Error> {
    // parse command line arguments
    let cmd = Command::new("puppy-rs")
        .about("a cute discord bot for various roleplay commands")
        .version("3.0.0")
        .author("PancakeTAS")
        .arg(
            Arg::new("verbose")
                .short('v')
                .long("verbose")
                .help("override the log level to 'trace'")
                .action(ArgAction::SetTrue)
        )
        .arg(
            Arg::new("config")
                .short('c')
                .long("config")
                .help("specify the configuration file to use")
                .action(ArgAction::Set)
                .num_args(1)
                .value_parser(clap::value_parser!(PathBuf))
        );

    let matches = cmd.get_matches();

    // read configuration file
    let default_path = PathBuf::from("config.yml");
    let config_path = matches.get_one::<PathBuf>("config").unwrap_or(&default_path);
    let config_file = File::open(config_path)
        .context("unable to open configuration file")?;
    let config: Configuration = serde_yml::from_reader(config_file)
        .context("unable to parse configuration file")?;

    // initialize the logger
    let level = if matches.get_flag("verbose") { LevelFilter::Trace } else { LevelFilter::from_str(&config.log_level).context("unable to parse log level")? };
    colog::default_builder()
        .filter_level(LevelFilter::Off) // don't filter anything by default
        .filter_module("module/reaction/backend/nekosbest", level)
        .filter_module("module/reaction/backend/otakugifs", level)
        .filter_module("module/reaction/backend", level)
        .filter_module("module/reaction", level)
        .filter_module("module/status", level)
        .filter_module("module/random", level)
        .filter_module("bot", level)
        .format(colog::formatter(CustomPrefixToken))
        .init();

    // create the bot
    bot::launch_bot(config).await?;

    Ok(())
}