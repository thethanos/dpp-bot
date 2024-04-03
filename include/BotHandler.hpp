#pragma once

#include "TokenStorage.hpp"

#include <dpp/dpp.h>

class BotHandler
{
public:
    BotHandler(const std::string& token):m_bot(token) {}

public:
    int init_data(const std::string& path_to_keys = "");
    void init_handlers();
    void start() { m_bot.start(dpp::st_wait); }

    void handle_ready(const dpp::ready_t& event);
    void handle_slashcommand(const dpp::slashcommand_t& event);
    void handle_button_click(const dpp::button_click_t& event);

private:
    dpp::cluster m_bot;
    TokenStorage m_tokens;
};