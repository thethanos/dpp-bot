#include "BotHandler.hpp"
#include "Config.hpp"

int main(int argc, char** argv) 
{    
    const char* token = std::getenv("BOT_TOKEN");
    if (token == nullptr) {
        spdlog::error("Could not find the bot token");
        return -1;
    }

    BotHandler bot(token);
    bot.init_data("../keys.csv");
    bot.init_handlers();
    bot.start();

    return 0;
}