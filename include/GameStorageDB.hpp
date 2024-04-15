#pragma once

#include <pqxx/pqxx>

#include "Game.hpp"
#include "Config.hpp"

#include <exception> 
#include <format>
#include <optional>
#include <unordered_map>

class GameStorageDB
{
public:
    GameStorageDB(const Config& config)
    {   
        try {
            m_dbConn = pqxx::connection(std::format("host={} port={} user={} password={} dbname={}",
                config.psql_host,
                config.psql_port,
                config.psql_user,
                config.psql_password,
                config.psql_dbname
            ));
        } catch (const std::exception& e) {
            throw std::runtime_error(std::format("Failed to open database: {}", e.what()));
        }   
    }

public:
    std::optional<const std::string> init_game_storage();
    std::optional<const std::string> insert_game(const Game& game);
    std::optional<const std::string> insert_games(const std::unordered_map<std::string, Game>& games);
    std::optional<const std::string> update_game(const Game& game);
    std::unordered_map<std::string, Game> select_games(const std::string& condition = "");

private:
    std::optional<const std::string> execute_simple_query(const std::string&);

private:
    std::optional<pqxx::connection> m_dbConn;
};