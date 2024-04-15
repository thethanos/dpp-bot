#pragma once

#include "Randomizer.hpp"
#include "GameStorageDB.hpp"

#include "rapidcsv.hpp"

#include <optional>

class GameStorage
{
public:
    GameStorage(std::unique_ptr<GameStorageDB> dbConn):m_dbConn(std::move(dbConn)){}

public:
    const std::optional<const std::string> init_game_storage();
    const std::optional<const std::string> load_games_from_file(const std::string& path);
    const std::optional<const std::string> load_games_from_db();

    const std::optional<const Game> play();
    const std::optional<const std::string> deactivate(Game game);
    const std::optional<Page> get_game_list_page(size_t page_number);

    const bool empty() const { return m_games.empty(); }

private:
    const std::optional<const std::string> update_game_list_pages();
    const std::optional<const std::string> update_available_games();

private:
    Randomizer m_randomizer;
    std::unordered_map<std::string, Game> m_games;
    std::vector<std::string> m_game_list_pages;
    std::mutex m_mutex;
    std::unique_ptr<GameStorageDB> m_dbConn;
};