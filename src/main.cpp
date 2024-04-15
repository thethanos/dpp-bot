#include "BotHandler.hpp"
#include "Config.hpp"

int main(int argc, char** argv) 
{    
    Config config;
    if (auto loaded = config.load("config.toml"); !loaded) {
        spdlog::error("Failed to load config file");
        return -1;
    }

    auto dbConn = std::make_shared<DBConnection>(config);
    auto games = std::make_unique<GameStorage>(dbConn);
    auto users = std::make_unique<UserDataStorage>(dbConn);

    BotHandler bot(config.token, std::move(games), std::move(users));
    bot.init_data(config.path_to_keys);
    bot.init_handlers();
    bot.start();

    return 0;
}