#include "DBConnection.hpp"
#include "DBTables.hpp"

#include <stdlib.h>

std::shared_ptr<DBConnection> DBConnection::m_dbConn = nullptr;

std::optional<const std::string> DBConnection::create_game_table()
{
    const std::string query = "CREATE TABLE IF NOT EXISTS game("\
        "STATUS INT NOT NULL," \
        "ID CHAR(20) PRIMARY KEY NOT NULL,"\
        "NAME CHAR(30) NOT NULL,"\
        "ACTIVATION_KEY CHAR(30) NOT NULL," \
        "PRICE REAL NOT NULL," \
        "PRIORITY INT NOT NULL" \
    ");";

    std::lock_guard guard{m_game_mutex};
    return execute_query(query);
}

std::optional<const std::string> DBConnection::create_user_score_table()
{
    const std::string query = "CREATE TABLE IF NOT EXISTS user_score(" \
        "ID CHAR(20) PRIMARY KEY NOT NULL," \
        "SCORE INT NOT NULL" \
    ");"; 

    std::lock_guard guard{m_user_score_mutex};
    return execute_query(query);
}

std::optional<const std::string> DBConnection::insert_game(const Game& game)
{
    std::string query = std::format("INSERT INTO game(STATUS, ID, NAME, ACTIVATION_KEY, PRICE, PRIORITY)" \
        "VALUES ({}, \"{}\", \"{}\", \"{}\", {}, {})",
        game.status,
        game.id,
        game.name,
        game.key,
        game.price,
        game.priority
    );

    std::lock_guard guard{m_game_mutex};
    return execute_query(query);
}

std::optional<const std::string> DBConnection::insert_games(const std::unordered_map<std::string, Game>& games)
{
    std::string values;
    for (auto [id, game] : games) {
        if (!values.empty()) {
            values += ",";
        }
        values += std::format("({}, \"{}\", \"{}\", \"{}\", {}, {})",
            game.status,
            game.id,
            game.name,
            game.key,
            game.price,
            game.priority
        );
    }

    std::lock_guard guard{m_game_mutex};
    return execute_query(std::format("INSERT INTO game(STATUS, ID, NAME, ACTIVATION_KEY, PRICE, PRIORITY) VALUES {};", values));
}

std::optional<const std::string> DBConnection::insert_user(const std::string& user_id)
{
    std::string query = std::format("INSERT INTO user_score(ID, SCORE) VALUES(\"{}\", {});",
        user_id,
        0
    );

    std::lock_guard guard{m_user_score_mutex};
    return execute_query(query);
}

std::optional<const std::string> DBConnection::update_game(const Game& game)
{
    std::string query = std::format(
        "UPDATE game SET " \
            "STATUS = {0},"
            "ID = \"{1}\"," \
            "NAME = \"{2}\"," \
            "ACTIVATION_KEY = \"{3}\"," \
            "PRICE = {4}," \
            "PRIORITY = {5} " \
        "WHERE ID = \"{1}\";", \
            game.status,
            game.id, 
            game.name, 
            game.key, 
            game.price, 
            game.priority
    );

    std::lock_guard guard{m_game_mutex};
    return execute_query(query);
}

std::optional<const std::string> DBConnection::update_user_score(const std::string& user_id, size_t score)
{
    std::string query = std::format("UPDATE user_score SET ID = \"{}\", SCORE = {}",
        user_id,
        score
    );

    std::lock_guard guard{m_user_score_mutex};
    return execute_query(query);
}

std::unordered_map<std::string, Game> DBConnection::select_games(const std::string& condition)
{  
    std::unordered_map<std::string, Game> games; 
    
    std::string query;
    if (!condition.empty()) {
        query = std::format("SELECT * FROM game {};", condition);
    } else {
        query = "SELECT * FROM game;";
    }

    sqlite3_stmt* stmt = nullptr;
    auto error_code = sqlite3_prepare_v2(m_sqlite, query.c_str(), -1, &stmt, nullptr);
    if (error_code != SQLITE_OK) {
        spdlog::error(std::format("sqlite3_prepare_v2: {}", sqlite3_errmsg(m_sqlite)));
        return games;
    }

    std::lock_guard guard{m_game_mutex};
    while(true) {
        int error_code = sqlite3_step(stmt);
        if (error_code == SQLITE_ERROR) {
            spdlog::error(std::format("sqlite3_step: {}", sqlite3_errmsg(m_sqlite)));
            return games;
        }

        if (error_code == SQLITE_DONE) {
            return games;
        }

        Game game;
        game.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, DB::Game::ID));
        game.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, DB::Game::NAME));
        game.key = reinterpret_cast<const char*>(sqlite3_column_text(stmt, DB::Game::KEY));
        game.price = sqlite3_column_double(stmt, DB::Game::PRICE);
        game.priority = sqlite3_column_int(stmt, DB::Game::PRIORITY);

        games[game.id] = game;
    }

    return games;
}

std::optional<size_t> DBConnection::select_user_score(const std::string& user_id)
{
    std::string query = std::format("SELECT * FROM user_score WHERE ID = \"{}\"", user_id);
    
    sqlite3_stmt* stmt = nullptr;
    auto error_code = sqlite3_prepare_v2(m_sqlite, query.c_str(), -1, &stmt, nullptr);
    if (error_code != SQLITE_OK) {
        spdlog::error(std::format("sqlite3_prepare_v2: {}", sqlite3_errmsg(m_sqlite)));
        return std::nullopt;
    }

    std::lock_guard guard{m_user_score_mutex};
    error_code = sqlite3_step(stmt);
    if (error_code == SQLITE_ERROR) {
        spdlog::error(std::format("sqlite3_step: {}", sqlite3_errmsg(m_sqlite)));
        return std::nullopt;
    }

    return sqlite3_column_int(stmt, DB::UserScore::SCORE);
}

std::optional<const std::string> DBConnection::execute_query(const std::string& query, Callback callback)
{
    spdlog::info("Executing query: {}", query);

    int error_code = sqlite3_exec(m_sqlite, query.c_str(), callback, nullptr, nullptr);
    if (error_code != SQLITE_OK) {
        spdlog::error("execute_query: {}", sqlite3_errmsg(m_sqlite));
        return sqlite3_errmsg(m_sqlite);
    }

    spdlog::info("Query executed successfully!");
    return std::nullopt;
}