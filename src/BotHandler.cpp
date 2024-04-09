#include "BotHandler.hpp"
#include "DBConnection.hpp"
#include "Message.hpp"

#include <algorithm>

int BotHandler::init_data(const std::string& path_to_keys)
{
    if (auto error = m_tokens.create_table(); error.has_value()) {
        spdlog::error(std::format("create_table: {}", error.value()));
        return -1;
    }

    if (auto error = m_users.create_table(); error.has_value()) {
        spdlog::error(std::format("create_table: {}", error.value()));
        return -1;
    }

    if (auto error = m_tokens.load_from_db(); error.has_value()) {
        spdlog::error(std::format("load_from_db: {}", error.value()));
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
    m_bot.on_message_create(std::bind(&BotHandler::handle_message_received, this, std::placeholders::_1));
}

void BotHandler::handle_ready(const dpp::ready_t& event)
{
    if (dpp::run_once<struct register_bot_commands>()) {
        m_bot.global_command_create(dpp::slashcommand("random", "Get random key!", m_bot.me.id));
        m_bot.global_command_create(dpp::slashcommand("games", "Get list of available games", m_bot.me.id));
        m_bot.global_command_create(dpp::slashcommand("score", "Get your balance", m_bot.me.id));
    }
}

void BotHandler::handle_slashcommand(const dpp::slashcommand_t& event)
{
    const std::string command_name = event.command.get_command_name();

    if (command_name == "random") {
        on_slashcommand_random(event);
    }

    if (command_name == "games") {
        on_slashcommand_games(event);
    }

    if (command_name == "score") {
        on_slashcommand_score(event);
    }
}

void BotHandler::handle_button_click(const dpp::button_click_t& event)
{
    const auto meta = get_event_meta(event);
    if (!meta.has_value()) {
        spdlog::error("get_event_meta: failed to aquire event meta");
        return;
    }

    const auto name = meta.value().parent.name;
    if (name == "random") {
        on_button_click_get_prize(event, meta.value().parent.user_id);
    }

    if (name == "games") {
        on_button_click_games(event);
    }
}

void BotHandler::handle_message_received(const dpp::message_create_t& event)
{   
    auto user_id = event.msg.author.id.str();
    m_users.add_score(user_id, 1);
}

void BotHandler::on_slashcommand_random(const dpp::slashcommand_t& event) 
{
    auto user_id = event.command.usr.id.str();
    auto user_score = m_users.get_score(user_id);
    if (!user_score.has_value() || user_score.value() == 0) {
        event.reply("Sorry, your balance is 0. Please try again later.");
        return;
    }

    m_users.remove_score(user_id, 1);

    std::optional<const Token> game = m_tokens.play();
    if (!game.has_value()) {
        spdlog::error("TokenStorage returned no keys");
        event.reply("Sorry, we are out of keys. Please try again later.");
        return;
    }

    dpp::message msg(event.command.channel_id, "Your game is " + game.value().name);
    msg.add_component(
        dpp::component().add_component(
            dpp::component()
                .set_label("Get the key")
                .set_type(dpp::cot_button)
                //.set_emoji(dpp::unicode_emoji::smile)
                .set_style(dpp::cos_danger)
                .set_id(game.value().id)
        )
    );
            
    event.reply(msg);
}

void BotHandler::on_slashcommand_games(const dpp::slashcommand_t& event)
{
    if (m_slashcommand_events.contains("gamse")) {
        m_slashcommand_events["gamse"].delete_original_response();
        m_slashcommand_events.erase("games");
    }

    auto list = m_tokens.get_list_page("");
    if (!list.has_value()) {
        spdlog::error("Failed to load a page");
        event.reply(dpp::message("No games available."));
        return;
    }
    
    dpp::embed embed;
    embed.set_title("Games");
    embed.set_description(list.value());

    auto msg = make_game_list_message(embed);
    event.reply(msg);

    m_slashcommand_events["games"] = event;
}

void BotHandler::on_slashcommand_score(const dpp::slashcommand_t& event)
{
    auto user_id = event.command.usr.id.str();
    auto user_score = m_users.get_score(user_id);
    if (!user_score.has_value()) {
        spdlog::error("get_score: empty score value returned");
        event.reply("Sorry, something went wrong. Please try again later.");
        return;
    }

    event.reply(dpp::message(std::format("You have {} points.", user_score.value())));
}

void BotHandler::on_button_click_get_prize(const dpp::button_click_t& event, const IdType& event_target)
{
    auto user_id = event.command.member.user_id;
    if (user_id.str() != event_target) {
        event.reply();
        return;
    }

    auto token = m_tokens.get_prize(event.custom_id);
    if (!token.has_value()) {
        spdlog::error("get_prize: failed to retrieve token by id");
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

void BotHandler::on_button_click_games(const dpp::button_click_t& event)
{
    auto list = m_tokens.get_list_page(event.custom_id);
    if (!list.has_value()) {
        event.reply();
        return;
    }

    dpp::embed embed;
    embed.set_title("Games");
    embed.set_description(list.value());
    auto msg = make_game_list_message(embed);

    m_slashcommand_events["games"].edit_response(msg);
    event.reply();
}