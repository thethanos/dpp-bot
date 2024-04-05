#pragma once

#include "Randomizer.hpp"
#include "DBConnection.hpp"

#include "rapidcsv.hpp"

#include <optional>

class TokenStorage
{
public:
    const std::optional<const std::string> load_from_file(const std::string& path);
    const std::optional<const std::string> load_from_db();

    const std::optional<const Token> play();
    const std::optional<const std::string> deactivate(Token token);
    const std::optional<Token> get_prize(const std::string& game_id);    

    const std::vector<Token> get_available_games();

    const bool empty() const { return m_tokens.empty(); }

private:
    Randomizer m_randomizer;
    std::unordered_map<std::string, Token> m_tokens;
};