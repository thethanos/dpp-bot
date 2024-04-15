#include "DBConnection.hpp"
#include "DBTables.hpp"

#include <stdlib.h>

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

    return execute_query(query);
}

std::optional<const std::string> DBConnection::create_user_score_table()
{
    const std::string query = "CREATE TABLE IF NOT EXISTS user_score(" \
        "ID CHAR(20) PRIMARY KEY NOT NULL," \
        "SCORE INT NOT NULL" \
    ");"; 

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

    return execute_query(std::format("INSERT INTO game(STATUS, ID, NAME, ACTIVATION_KEY, PRICE, PRIORITY) VALUES {};", values));
}

std::optional<const std::string> DBConnection::insert_user(const std::string& user_id)
{
    std::string query = std::format("INSERT INTO user_score(ID, SCORE) VALUES(\"{}\", {});",
        user_id,
        0
    );

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

    return execute_query(query);
}

std::optional<const std::string> DBConnection::update_user_score(const std::string& user_id, size_t score)
{
    std::string query = std::format("UPDATE user_score SET ID = \"{}\", SCORE = {}",
        user_id,
        score
    );

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

    pqxx::work trx(m_dbConn);
    try {
        auto result = trx.exec(query);
        for (auto row : result) {
            Game game;
            game.id = row[DB::Game::ID].as<std::string>();
            game.name = row[DB::Game::NAME].as<std::string>();
            game.key = row[DB::Game::KEY].as<std::string>();
            game.price = row[DB::Game::PRICE].as<double>();
            game.priority = row[DB::Game::PRIORITY].as<int>();

            games[game.id] = game;
        }
    } catch (const std::exception& e) {
        spdlog::error("select_games: {}", e.what());
        return games;
    }

    trx.commit();
    return games;
}

std::optional<size_t> DBConnection::select_user_score(const std::string& user_id)
{
    std::string query = std::format("SELECT * FROM user_score WHERE ID = \"{}\"", user_id);
    
    pqxx::work trx(m_dbConn);
    pqxx::result result;
    try {
        result = trx.exec(query);
    } catch (const std::exception& e) {
        spdlog::error("select_user_score: {}", e.what());
        return std::nullopt;
    }

    trx.commit();
    return result.at(0, DB::UserScore::SCORE).as<int>();
}

std::optional<const std::string> DBConnection::execute_query(const std::string& query, Callback callback)
{
    spdlog::info("Executing query: {}", query);

    pqxx::work trx(m_dbConn);
    try {
        trx.exec(query);
    } catch (const std::exception& e) {
        spdlog::error("execute_query: {}", e.what());
        return e.what();
    }

    trx.commit();
    spdlog::info("Query executed successfully!");
    return std::nullopt;
}