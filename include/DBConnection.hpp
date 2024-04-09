#pragma once

#include "sqlite3.h"
#include "spdlog/spdlog.h"

#include "Game.hpp"

#include <exception> 
#include <memory>
#include <format>
#include <mutex>
#include <optional>
#include <functional>
#include <unordered_map>

using Callback = int(void*,int,char**,char**);

class DBConnection
{
public:
    ~DBConnection()
    {
        sqlite3_close(m_sqlite);
    }

protected:
    DBConnection()
    {
        int error_code = sqlite3_open("test.db", &m_sqlite);
        if (error_code != 0) {
            throw std::runtime_error(std::format("Failed to open database, error: {}", sqlite3_errmsg(m_sqlite)));
        }
    }

public:
    // get a pointer to an open data base connection
    static std::shared_ptr<DBConnection> get_conn()
    {
        if (m_dbConn == nullptr) {
            m_dbConn = std::shared_ptr<DBConnection>(new DBConnection());
            spdlog::info("Database connection opened successfully");
        }

        return m_dbConn;
    }

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
    std::optional<const std::string> execute_query(const std::string&, Callback callback = nullptr);

private:
    std::mutex m_game_mutex;
    std::mutex m_user_score_mutex;
    sqlite3* m_sqlite{nullptr};
    static std::shared_ptr<DBConnection> m_dbConn;
};