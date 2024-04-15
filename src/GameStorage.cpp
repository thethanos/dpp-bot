#include "GameStorage.hpp"

#include <algorithm>

const std::optional<const std::string> GameStorage::init_game_storage()
{
    if (auto error = m_dbConn->init_game_storage(); error.has_value()) {
        return std::format("create_game_table: {}", error.value());
    }

    return std::nullopt;
}

const std::optional<const std::string> GameStorage::load_games_from_file(const std::string& path) 
{   
    rapidcsv::Document doc(path, rapidcsv::LabelParams(-1, -1));
    if (doc.GetRowCount() == 0) {
        return std::format("GetRowCount: {}; The file is empty", path);
    }

    for (size_t row(0); row < doc.GetRowCount(); row++) 
    {
        auto status = doc.GetCell<size_t>(STATUS, row);
        if (status != ACTIVE) {
            continue;
        }

        Game game;
        game.status = status;
        game.id = doc.GetCell<std::string>(ID, row);
        game.name = doc.GetCell<std::string>(NAME, row);
        game.key = doc.GetCell<std::string>(KEY, row);
        game.price = doc.GetCell<double>(PRICE, row);
        game.priority = doc.GetCell<size_t>(PRIORITY, row);

        m_games[game.id] = game;
    }

    if (auto error = m_dbConn->insert_games(m_games); error.has_value()) {
        return std::format("insert_games: {}", error.value());
    }

    update_game_list_pages();
    m_randomizer.load_ids(m_games);
    return std::nullopt;
}

const std::optional<const std::string> GameStorage::load_games_from_db()
{   
    update_available_games();
    m_randomizer.load_ids(m_games);
    return std::nullopt;
}

const std::optional<const Game> GameStorage::play()
{
    auto id = m_randomizer.get_random_id();
    if (!id.has_value()) {
        return std::nullopt;
    }

    auto gameIt = m_games.find(id.value());
    if (gameIt == m_games.end()) {
        return std::nullopt;
    }

    return gameIt->second;
}

const std::optional<const std::string> GameStorage::deactivate(Game game)
{   
    game.status = INACTIVE;
    auto error = m_dbConn->update_game(game);
    if (error.has_value()) {
        return std::format("update_game: {}", error.value());
    }

    update_available_games();
    spdlog::info("{} is deactivated and removed from random", game.name);
    return std::nullopt;
}

const std::optional<Page> GameStorage::get_game_list_page(size_t page_number)
{
    std::lock_guard guard(m_mutex);
    if (m_game_list_pages.empty()) {
        return std::nullopt;
    }

    if (page_number >= m_game_list_pages.size()) {
        return Page{m_game_list_pages[0], 0};
    }

    return Page{m_game_list_pages[page_number], page_number};
}

const std::optional<const std::string> GameStorage::update_game_list_pages()
{   
    std::lock_guard guard(m_mutex);
    m_game_list_pages.clear();

    size_t counter = 0;
    std::string page;
    for (auto [id, game] : m_games) {
        if (counter == m_games.size() - 1) {
            m_game_list_pages.push_back(page);
            return std::nullopt;
        }
        if (counter > 0 && counter % 10 == 0) {
            m_game_list_pages.push_back(page);
            page.clear();
        }
        page += std::format("{}. {}\n", counter + 1, game.name);
        counter++;
    }
    return std::nullopt;
}

const std::optional<const std::string> GameStorage::update_available_games()
{
    auto games = m_dbConn->select_games(std::format("WHERE STATUS = {} ORDER BY NAME", int(ACTIVE)));
    if (games.empty()) {
        return "select_games: failed to load games";
    }

    m_games = std::move(games);
    return update_game_list_pages();
}