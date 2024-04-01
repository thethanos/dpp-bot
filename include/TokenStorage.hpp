#pragma once

#include "randomizer.hpp"

#include <optional>

class TokenStorage
{
public:
    void read_file(const std::string& path) 
    {   
        rapidcsv::Document doc(path, rapidcsv::LabelParams(-1, -1));

        for (size_t row(0); row < doc.GetRowCount(); row++) 
        {
            auto status = doc.GetCell<size_t>(STATUS, row);
            if (status != ACTIVE) {
                continue;
            }

            Token token;
            token.id = doc.GetCell<std::string>(ID, row);
            token.name = doc.GetCell<std::string>(NAME, row);
            token.key = doc.GetCell<std::string>(KEY, row);
            token.price = doc.GetCell<double>(PRICE, row);
            token.priority = doc.GetCell<size_t>(PRIORITY, row);

            m_tokens[token.id] = token;
        }

        m_randomizer.load_ids(m_tokens);
    }

    const std::optional<std::string> play(const std::string& user_id)
    {
        const std::string id = m_randomizer.get_random_id();
        if (id.empty()) {
            return std::nullopt;
        }

        auto tokenIt = m_tokens.find(id);
        if (tokenIt == m_tokens.end()) {
            return std::nullopt;
        }
        
        m_winners[user_id] = tokenIt->second;
        return tokenIt->second.name;
    }

    const std::optional<Token> get_prize(const std::string& user_id)
    {
        auto prizeIt = m_winners.find(user_id);
        if (prizeIt == m_winners.end()) {
            return std::nullopt;
        }

        return prizeIt->second;
    }

private:
    Randomizer m_randomizer;
    std::unordered_map<std::string, Token> m_tokens;
    std::unordered_map<std::string, Token> m_winners;
};