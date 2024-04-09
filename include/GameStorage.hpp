#pragma once

#include "Randomizer.hpp"
#include "DBConnection.hpp"

#include "rapidcsv.hpp"

#include <optional>

class GameStorage
{
public:
    const std::optional<const std::string> create_table();
    const std::optional<const std::string> load_games_from_file(const std::string& path);
    const std::optional<const std::string> load_games_from_db();

    const std::optional<const Game> play();
    const std::optional<const std::string> deactivate(Game game);
    const std::optional<Game> get_prize(const std::string& game_id);    

    const std::optional<const std::string> get_game_list_page(const std::string& direction);

    const bool empty() const { return m_games.empty(); }

private:
    const std::optional<const std::string> update_game_list_pages();
    const std::optional<const std::string> update_available_games();

private:
    Randomizer m_randomizer;
    std::unordered_map<std::string, Game> m_games;
    std::vector<std::string> m_game_list_pages;
    int m_current_page = 0;
    std::mutex m_mutex;
};