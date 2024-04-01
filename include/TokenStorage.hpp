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

    const std::optional<const std::string> play(const std::string& user_id);
    const std::optional<Token> get_prize(const std::string& user_id);

private:
    Randomizer m_randomizer;
    std::unordered_map<std::string, Token> m_tokens;
    std::unordered_map<std::string, Token> m_winners;
};