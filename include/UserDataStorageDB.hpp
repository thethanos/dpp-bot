#pragma once

#include <pqxx/pqxx>

#include "Game.hpp"
#include "Config.hpp"

#include <exception> 
#include <format>
#include <optional>

class UserDataStorageDB
{
public:
    UserDataStorageDB(const Config& config)
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
    std::optional<const std::string> init_user_data_storage();
    std::optional<const std::string> insert_user(const std::string& user_id);
    std::optional<const std::string> update_user_score(const std::string& user_id, size_t score);
    std::optional<size_t> select_user_score(const std::string& user_id);

private:
    std::optional<const std::string> execute_simple_query(const std::string&);

private:
    std::optional<pqxx::connection> m_dbConn;
};