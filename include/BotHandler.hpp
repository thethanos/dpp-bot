#pragma once

#include "GameStorage.hpp"
#include "EventParser.hpp"
#include "UserDataStorage.hpp"

#include <dpp/dpp.h>

class BotHandler
{
public:
    BotHandler(const Config& cfg, 
        std::unique_ptr<GameStorage> games, 
        std::unique_ptr<UserDataStorage> users)
        :m_cfg(cfg)
        ,m_bot(cfg.token,
            dpp::i_message_content | 
            dpp::i_guilds | 
            dpp::i_guild_members | 
            dpp::i_guild_messages)
        ,m_games(std::move(games))
        ,m_users(std::move(users))
    {

    }

public:
    int init_data();
    void init_handlers();
    void start() { m_bot.start(dpp::st_wait); }

private:
    void handle_ready(const dpp::ready_t& event);
    void handle_slashcommand(const dpp::slashcommand_t& event);
    void handle_button_click(const dpp::button_click_t& event);
    void handle_message_received(const dpp::message_create_t& event);

    void on_slashcommand_random(const dpp::slashcommand_t& event);
    void on_slashcommand_games(const dpp::slashcommand_t& event);
    void on_slashcommand_score(const dpp::slashcommand_t& event);

    void on_button_click_random(const dpp::button_click_t& event, const EventMeta& meta);
    void on_button_click_games(const dpp::button_click_t& event, const EventMeta& meta);

private:
    Config m_cfg;
    dpp::cluster m_bot;
    std::unique_ptr<GameStorage> m_games;
    std::unique_ptr<UserDataStorage> m_users;
    dpp::slashcommand_t event;
    std::unordered_map<std::string, dpp::slashcommand_t> m_slashcommand_events;
};