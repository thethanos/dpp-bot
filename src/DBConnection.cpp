#include "DBConnection.hpp"
#include "DBTables.hpp"

#include <stdlib.h>

std::optional<const std::string> DBConnection::create_game_table()
{
    const std::string query = "CREATE TABLE IF NOT EXISTS game("\
        "STATUS INT NOT NULL," \
        "ID CHAR(20) PRIMARY KEY NOT NULL,"\
        "NAME CHAR(50) NOT NULL,"\
        "ACTIVATION_KEY CHAR(30) NOT NULL," \
        "PRICE REAL NOT NULL," \
        "PRIORITY INT NOT NULL" \
    ");";

    return execute_simple_query(query);
}

std::optional<const std::string> DBConnection::create_user_score_table()
{
    const std::string query = "CREATE TABLE IF NOT EXISTS user_score(" \
        "ID CHAR(20) PRIMARY KEY NOT NULL," \
        "SCORE INT NOT NULL" \
    ");"; 

    return execute_simple_query(query);
}

std::optional<const std::string> DBConnection::insert_game(const Game& game)
{
    std::string query = "INSERT INTO game(STATUS, ID, NAME, ACTIVATION_KEY, PRICE, PRIORITY) VALUES ($1, $2, $3, $4, $5, $6)";

    pqxx::work trx(m_dbConn.value());
    try {
        trx.exec_params0(query, game.status, game.id, game.name, game.key, game.price, game.priority);
    } catch (const std::exception& e) {
        spdlog::error("insert_game: {}", e.what());
        return e.what();
    }

    trx.commit();
    return std::nullopt;
}

std::optional<const std::string> DBConnection::insert_games(const std::unordered_map<std::string, Game>& games)
{
    pqxx::work trx(m_dbConn.value());    
    try {
        auto stream = pqxx::stream_to::table(
            trx, 
            pqxx::table_path({"game"}), 
            {"status", "id", "name", "activation_key", "price", "priority"}
        );

        for (auto [id, game] : games) {
            stream << std::tie(game.status,game.id, game.name, game.key, game.price, game.priority);
        }

        stream.complete();
    } catch (const std::exception& e) {
        spdlog::error("insert_games: {}", e.what());
        return e.what();
    }

    trx.commit();
    return std::nullopt;
}

std::optional<const std::string> DBConnection::insert_user(const std::string& user_id)
{
    std::string query = "INSERT INTO user_score(ID, SCORE) VALUES($1, $2);";

    pqxx::work trx(m_dbConn.value());
    try {
        trx.exec_params0(query, user_id, 0);
    } catch (const std::exception& e) {
        spdlog::error("insert_user: {}", e.what());
        return e.what();
    }

    trx.commit();
    return std::nullopt;
}

std::optional<const std::string> DBConnection::update_game(const Game& game)
{
    std::string query = \
        "UPDATE game SET " \
            "STATUS = $1," \
            "ID = $2," \
            "NAME = $3," \
            "ACTIVATION_KEY = $4," \
            "PRICE = $5," \
            "PRIORITY = $6 " \
        "WHERE ID = $2;";

    pqxx::work trx(m_dbConn.value());
    try {
        trx.exec_params0(query, game.status, game.id, game.name, game.key, game.price, game.priority);
    } catch (const std::exception& e) {
        spdlog::error("update_game: {}", e.what());
        return e.what();
    }

    trx.commit();
    return std::nullopt;
}

std::optional<const std::string> DBConnection::update_user_score(const std::string& user_id, size_t score)
{
    std::string query = "UPDATE user_score SET ID = $1, SCORE = $2";

    pqxx::work trx(m_dbConn.value());
    try {
        trx.exec_params0(query, user_id, score);
    } catch (const std::exception& e) {
        spdlog::error("update_user_score: {}", e.what());
        return e.what();
    }

    trx.commit();
    return std::nullopt;
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

    pqxx::work trx(m_dbConn.value());
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
    pqxx::work trx(m_dbConn.value());
    std::string query = "SELECT score FROM user_score WHERE ID =" + trx.quote(user_id);
    size_t result(0);
    try {

        result = trx.query_value<int>(query);
    } catch (const std::exception& e) {
        spdlog::error("select_user_score: {}", e.what());
        return std::nullopt;
    }

    trx.commit();
    return result;
}

std::optional<const std::string> DBConnection::execute_simple_query(const std::string& query)
{
    spdlog::info("Executing query: {}", query);

    pqxx::work trx(m_dbConn.value());
    try {
        trx.exec(query);
    } catch (const std::exception& e) {
        spdlog::error("execute_simple_query: {}", e.what());
        return e.what();
    }

    trx.commit();
    spdlog::info("Query executed successfully!");
    return std::nullopt;
}