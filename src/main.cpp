#include "BotHandler.hpp"
#include "Config.hpp"

int main(int argc, char** argv) 
{    
    Config config;
    if (auto loaded = config.load("config.toml"); !loaded) {
        spdlog::error("Failed to load config file");
        return -1;
    }

    auto gameDb = std::make_unique<GameStorageDB>(config);
    auto games = std::make_unique<GameStorage>(std::move(gameDb));

    auto userDb = std::make_unique<UserDataStorageDB>(config);
    auto users = std::make_unique<UserDataStorage>(std::move(userDb));

    BotHandler bot(config.token, std::move(games), std::move(users));
    bot.init_data(config.path_to_keys);
    bot.init_handlers();
    bot.start();

    return 0;
}