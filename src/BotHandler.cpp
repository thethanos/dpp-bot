#include "BotHandler.hpp"
#include "DBConnection.hpp"

#include <algorithm>

int BotHandler::init_data(const std::string& path_to_keys)
{
    if (auto error = DBConnection::get_conn()->create_token_table(); error.has_value()) {
        spdlog::error(error.value());
        return -1;
    }

    if (auto error = m_tokens.load_from_db(); error.has_value()) {
        spdlog::error(error.value());
    }
    
    if (m_tokens.empty()) {
        spdlog::info("init_data: trying to load from file..");
        if (auto error = m_tokens.load_from_file(path_to_keys); error.has_value()) {
            spdlog::error(error.value());
            return -1;
        }
    }

    m_bot.on_log([](const dpp::log_t& log){
        switch(log.severity) {
            case dpp::loglevel::ll_info:
                spdlog::info(log.message);
                break;
            case dpp::loglevel::ll_warning:
                spdlog::warn(log.message);
                break;
            case dpp::loglevel::ll_error:
                spdlog::error(log.message);
                break;
            default:
                spdlog::trace(log.message);
        } 
    });
    return 0;
}

void BotHandler::init_handlers()
{
    m_bot.on_ready(std::bind(&BotHandler::handle_ready, this, std::placeholders::_1));
    m_bot.on_slashcommand(std::bind(&BotHandler::handle_slashcommand, this, std::placeholders::_1));
    m_bot.on_button_click(std::bind(&BotHandler::handle_button_click, this, std::placeholders::_1));
}

void BotHandler::handle_ready(const dpp::ready_t& event)
{
    if (dpp::run_once<struct register_bot_commands>()) {
        m_bot.global_command_create(dpp::slashcommand("random", "Get random key!", m_bot.me.id));
        m_bot.global_command_create(dpp::slashcommand("list", "Get list of available games", m_bot.me.id));
    }
}

void BotHandler::handle_slashcommand(const dpp::slashcommand_t& event)
{
    if (event.command.get_command_name() == "random") {
        auto user_id = event.command.usr.id.str();
        std::optional<std::string> game_name = m_tokens.play(user_id);
        if (!game_name.has_value()) {
            spdlog::error("TokenStorage returned no keys");
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

    if (event.command.get_command_name() == "list") {
        
    }
}

void BotHandler::handle_button_click(const dpp::button_click_t& event)
{
    auto user_id = event.command.member.user_id;
    if (user_id.str() != event.custom_id) {
        event.reply();
        return;
    }

    auto token = m_tokens.get_prize(user_id.str());
    if (!token.has_value()) {
        spdlog::error("Failed to retrieve token by id");
        m_bot.message_create(dpp::message(event.command.channel_id, "Unknown error. Please try again later"));
        event.reply();
        event.delete_original_response();
        return;
    }
 
    m_bot.direct_message_create(user_id, dpp::message("Here is your key: " + token.value().key));
    m_bot.message_create(dpp::message(event.command.channel_id, "Check your key in DM"));

    event.reply();
    event.delete_original_response();
    m_tokens.deactivate(token.value());
}