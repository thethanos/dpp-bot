#pragma once

#include "Token.hpp"

#include <unordered_map>
#include <vector>
#include <random>
#include <mutex>
#include <algorithm>

class Randomizer
{
public:
    void load_ids(const std::unordered_map<std::string, Token>& tokens)
    {   
        std::lock_guard guard(m_mutex);
        m_token_ids.clear();
        for (auto [id, token] : tokens)
        {
            for (size_t count(0); count < token.priority; count++) {
                m_token_ids.push_back(token.id);
            }
        }
    }

    std::string get_random_id()
    {
        if (m_token_ids.size() == 0) {
            return std::string{};
        }

        std::default_random_engine generator;
        std::random_device rd;
        generator.seed(rd());

        std::lock_guard guard(m_mutex);
        std::uniform_int_distribution<size_t> distribution(0, m_token_ids.size());

        size_t index = distribution(generator);
        std::string token_id = m_token_ids[index];

        auto remove_iter = std::remove(m_token_ids.begin(), m_token_ids.end(), token_id);
        m_token_ids.erase(remove_iter, m_token_ids.end());

        return token_id;
    }

private:
    std::mutex m_mutex;
    std::vector<std::string> m_token_ids;
};

