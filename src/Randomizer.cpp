#include "Randomizer.hpp"
#include "spdlog/spdlog.h"

#include <random>
#include <algorithm>

void Randomizer::load_ids(const std::unordered_map<std::string, Game>& games)
{   
    std::lock_guard guard(m_mutex);
    m_game_ids.clear();
    for (auto [id, game] : games)
    {
        for (size_t count(0); count < game.priority; count++) {
            m_game_ids.push_back(game.id);
        }
    }
}

std::optional<const std::string> Randomizer::get_random_id()
{
    if (m_game_ids.empty()) {
        spdlog::error("get_random_id: m_game_ids vector is empty");
        return std::nullopt;
    }

    std::default_random_engine generator;
    std::random_device rd;
    generator.seed(rd());

    std::lock_guard guard(m_mutex);
    std::uniform_int_distribution<size_t> distribution(0, m_game_ids.size());

    size_t index = distribution(generator);
    std::string game_id = m_game_ids[index];

    auto remove_iter = std::remove(m_game_ids.begin(), m_game_ids.end(), game_id);
    m_game_ids.erase(remove_iter, m_game_ids.end());

    return game_id;
}