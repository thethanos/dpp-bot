#include "BotHandler.hpp"
#include "Config.hpp"
#include <map>


int main(int argc, char** argv) 
{    
    Config config;
    if (auto loaded = config.load("config.toml"); !loaded) {
        spdlog::error("Failed to load config file");
        return -1;
    }

    BotHandler bot(config.token);
    bot.init_data(config.path_to_keys);
    bot.init_handlers();
    bot.start();

    return 0;
}