#include "BotHandler.hpp"
#include "spdlog/spdlog.h"

int main(int argc, char** argv) 
{    
    const char* token = std::getenv("BOT_TOKEN");
    if (token == nullptr) {
        std::cout << "Could not find the bot token.\n";
        return -1;
    }

    BotHandler bot(token);
    bot.init_data();
    bot.init_handlers();
    bot.start();

    return 0;
}