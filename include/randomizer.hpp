#pragma once

#include "Token.hpp"

#include <unordered_map>
#include <vector>
#include <mutex>
#include <optional>

class Randomizer
{
public:
    void load_ids(const std::unordered_map<std::string, Token>& tokens);
    std::optional<const std::string> get_random_id();

private:
    std::mutex m_mutex;
    std::vector<std::string> m_token_ids;
};

