# puppy-rs
puppy-rs is a modular discord bot written in Rust made for roleplaying with your friends, or even just showing them how much you like them.

## Modules
As of right now, puppy-rs has three modules, the first one being the `reaction` module.

With /reaction \<name of reaction>, you can react to other people's messages with an animated gif, fetched from one of the many backends puppy-rs supports.
Additionally, some commands such as /kiss have been given aliases for easier access.

The second module is the `random` module. Using /random \<coinflip/dice/number> you can let randomness decide your fate.

Lastly, the `status` module simply adds a status message to the bot's profile,
which is updated every 5 minutes.

## Usage
If you want to use puppy-rs, you can add it either to your guild or directly to your account using [this link](https://discord.com/oauth2/authorize?client_id=1180972444379517029).

If you add it directly to your account, you can use the bot in any guild you have the necessary permissions in as well as direct messages and group chats.

## Installation
While you can host your own instance of puppy-rs, I highly recommend using the public instance, as it's always up to date and maintained.
If you do however want to host your own instance, you can do so by cloning this repository and running `cargo run --release`. Please make sure you credit me if you do so.
