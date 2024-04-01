#include <dpp/dpp.h>
#include <dpp/unicode_emoji.h>

#include <iostream>
#include <algorithm>

#include "TokenStorage.hpp"
#include "DBConnection.hpp"

int main(int argc, char** argv) {

    auto db = DBConnection::get_conn();

    TokenStorage tokens;
    tokens.read_file("../keys.csv");
    
    const char* token = std::getenv("BOT_TOKEN");
    if (token == nullptr) {
        std::cout << "Could not find the bot token.\n";
        return -1;
    }

    dpp::cluster bot(token);
 
    bot.on_log(dpp::utility::cout_logger());
 
    bot.on_slashcommand([&tokens](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "random") {
            auto user_id = event.command.usr.id.str();
            std::optional<std::string> game_name = tokens.play(user_id);
            if (!game_name.has_value()) {
                event.reply("Sorry, we are out of keys. Please try again later.");
                return;
            }

            dpp::message msg(event.command.channel_id, "Your game is " + game_name.value());
            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                        .set_label("Get the key")
                        .set_type(dpp::cot_button)
                        //.set_emoji(dpp::unicode_emoji::smile)
                        .set_style(dpp::cos_danger)
                        .set_id(event.command.usr.id.str())
                )
            );
            
            event.reply(msg);
        }
    });

    bot.on_button_click([&bot, &tokens](const dpp::button_click_t& event) {
        auto user_id = event.command.member.user_id;
        if (user_id.str() != event.custom_id) {
            event.reply();
            return;
        }

        auto token = tokens.get_prize(user_id.str());
        if (!token.has_value()) {
            bot.message_create(dpp::message(event.command.channel_id, "Unknown error. Please try again later"));
            event.reply();
            event.delete_original_response();
            return;
        }
 
        bot.direct_message_create(user_id, dpp::message("Here is your key: " + token.value().key));
        bot.message_create(dpp::message(event.command.channel_id, "Check your key in DM"));

        event.reply();
        event.delete_original_response();
    });
    
    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(dpp::slashcommand("random", "Get random key!", bot.me.id));
        }
    });
 
    bot.start(dpp::st_wait);
    return 0;
}