#include "BotHandler.hpp"
#include "Message.hpp"

#include <algorithm>

int BotHandler::init_data(const std::string& path_to_keys)
{
    if (auto error = m_games->init_game_storage(); error.has_value()) {
        spdlog::error(std::format("init_game_storage: {}", error.value()));
        return -1;
    }

    if (auto error = m_users->init_user_data_storage(); error.has_value()) {
        spdlog::error(std::format("init_user_data_storage: {}", error.value()));
        return -1;
    }

    if (auto error = m_games->load_games_from_db(); error.has_value()) {
        spdlog::error(std::format("load_games_from_db: {}", error.value()));
    }
    
    if (m_games->empty()) {
        spdlog::info("init_data: trying to load from file..");
        if (auto error = m_games->load_games_from_file(path_to_keys); error.has_value()) {
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
    if (name == "random" && meta.value().user_id == meta.value().parent.user_id) {
        on_button_click_random(event, meta.value());
        return;
    }

    if (name == "games" && meta.value().user_id == meta.value().parent.user_id) {
        on_button_click_games(event, meta.value());
        return;
    }

    event.reply();
}

void BotHandler::handle_message_received(const dpp::message_create_t& event)
{   
    auto user_id = event.msg.author.id.str();
    m_users->add_score(user_id, 1);
}

void BotHandler::on_slashcommand_random(const dpp::slashcommand_t& event) 
{
    auto meta = get_event_meta(event);
    if (!meta.has_value()) {
        spdlog::error("get_event_meta: failed to parse event meta");
        event.reply("Error occured. Please try again later");
        return;
    }

    auto user_id = meta.value().user_id;

    auto user_score = m_users->get_score(user_id);
    if (!user_score.has_value() || user_score.value() < 10) {
        event.reply(std::format("Sorry, your balance is {}. Please try again later.", user_score.has_value()?user_score.value():0));
        return;
    }

    auto game = m_games->play();
    if (!game.has_value()) {
        spdlog::error("play: function returned no keys");
        event.reply("Sorry, we are out of keys. Please try again later.");
        return;
    }

    if (auto error = m_users->add_win(user_id, meta.value().event_id, game.value()); error.has_value()) {
        spdlog::error("add_win: failed to parse event meta");
        event.reply("Error occured. Please try again later");
        return;
    }

    auto msg = make_random_response_message(event.command.channel_id, game.value().name);       
    event.reply(msg);
}

void BotHandler::on_slashcommand_games(const dpp::slashcommand_t& event)
{
    if (m_slashcommand_events.contains("games")) {
        m_slashcommand_events["games"].delete_original_response();
        m_slashcommand_events.erase("games");
    }

    auto page = m_games->get_game_list_page(0);
    if (!page.has_value()) {
        spdlog::error("Failed to load a page");
        event.reply(dpp::message("No games available."));
        return;
    }
    
    dpp::embed embed;
    embed.set_title("Games");
    embed.set_description(page.value().content);

    auto msg = make_game_list_message(embed);
    event.reply(msg);

    m_slashcommand_events[event.command.id.str()] = event;
}

void BotHandler::on_slashcommand_score(const dpp::slashcommand_t& event)
{
    auto user_id = event.command.usr.id.str();
    auto user_score = m_users->get_score(user_id);
    if (!user_score.has_value()) {
        spdlog::error("get_score: empty score value returned");
        event.reply("Sorry, something went wrong. Please try again later.");
        return;
    }

    event.reply(dpp::message(std::format("You have {} points.", user_score.value())));
}

void BotHandler::on_button_click_random(const dpp::button_click_t& event, const EventMeta& meta)
{
    if (event.custom_id == "get") {
        auto game = m_users->get_win(meta.user_id, meta.parent.event_id);
        if (!game.has_value()) {
            spdlog::error("get_prize: failed to retrieve game by id");
            m_bot.message_create(dpp::message(event.command.channel_id, "Unknown error. Please try again later"));
            event.reply();
            event.delete_original_response();
            return;
        }
    
        m_bot.direct_message_create(meta.user_id, dpp::message("Here is your key: " + game.value().key));
        m_bot.message_create(dpp::message(event.command.channel_id, "Check your key in DM"));
        event.reply();
        event.delete_original_response();
        m_games->deactivate(game.value());
        m_users->remove_score(meta.user_id, 10);
        return;
    }

    if (event.custom_id == "sell") {
        m_bot.message_create(dpp::message(event.command.channel_id, "You sold the game for half of the price"));
        event.reply();
        event.delete_original_response();
        m_users->remove_score(meta.user_id, 5);
        return;
    }
}

void BotHandler::on_button_click_games(const dpp::button_click_t& event, const EventMeta& meta)
{   
    auto cursor = m_users->get_cursor(meta.user_id, meta.parent.event_id, event.custom_id);
    auto page = m_games->get_game_list_page(cursor);
    if (!page.has_value()) {
        event.reply();
        return;
    }
    m_users->set_cursor(meta.user_id, meta.parent.event_id, page.value().number);

    dpp::embed embed;
    embed.set_title("Games");
    embed.set_description(page.value().content);
    auto msg = make_game_list_message(embed);

    m_slashcommand_events[meta.parent.event_id].edit_response(msg);
    event.reply();
}