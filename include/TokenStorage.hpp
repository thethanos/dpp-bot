#pragma once

#include "Randomizer.hpp"
#include "DBConnection.hpp"

#include "rapidcsv.hpp"

#include <optional>

class TokenStorage
{
public:
    const std::optional<const std::string> create_table();
    const std::optional<const std::string> load_from_file(const std::string& path);
    const std::optional<const std::string> load_from_db();

    const std::optional<const Token> play();
    const std::optional<const std::string> deactivate(Token token);
    const std::optional<Token> get_prize(const std::string& game_id);    

    const std::optional<const std::string> get_list_page(const std::string& direction);

    const bool empty() const { return m_tokens.empty(); }

private:
    const std::optional<const std::string> update_available_games();

private:
    Randomizer m_randomizer;
    std::unordered_map<std::string, Token> m_tokens;
    std::vector<std::string> m_token_name_pages;
    int m_page_cursor = 0;
    std::mutex m_mutex;
};