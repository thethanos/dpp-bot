#pragma once

#include "spdlog/spdlog.h"
#include <pqxx/pqxx>

#include "Game.hpp"
#include "Config.hpp"

#include <exception> 
#include <memory>
#include <format>
#include <optional>
#include <unordered_map>

class DBConnection
{
public:
    DBConnection(const Config& config)
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
    std::optional<const std::string> create_game_table();
    std::optional<const std::string> create_user_score_table();
    std::optional<const std::string> insert_game(const Game& game);
    std::optional<const std::string> insert_games(const std::unordered_map<std::string, Game>& games);
    std::optional<const std::string> insert_user(const std::string& user_id);
    std::optional<const std::string> update_game(const Game& game);
    std::optional<const std::string> update_user_score(const std::string& user_id, size_t score);
    std::unordered_map<std::string, Game> select_games(const std::string& condition = "");
    std::optional<size_t> select_user_score(const std::string& user_id);

private:
    std::optional<const std::string> execute_simple_query(const std::string&);

private:
    std::optional<pqxx::connection> m_dbConn;
};