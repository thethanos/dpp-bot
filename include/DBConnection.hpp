#pragma once

#include "sqlite3.h"
#include "spdlog/spdlog.h"

#include "Token.hpp"

#include <exception> 
#include <memory>
#include <format>
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
            throw std::runtime_error(std::format("Failed to open data base, error: {}", sqlite3_errmsg(m_sqlite)));
        }
    }

public:
    // get a pointer to an open data base connection
    static std::shared_ptr<DBConnection> get_conn()
    {
        if (m_dbConn == nullptr) {
            m_dbConn = std::shared_ptr<DBConnection>(new DBConnection());
            spdlog::info("Data base connection opened successfully");
        }

        return m_dbConn;
    }

    std::optional<const std::string> create_token_table();
    std::optional<const std::string> insert_token(const Token&);
    std::unordered_map<std::string, Token> select_tokens(const std::string& condition = "");

private:
    std::optional<const std::string> execute_query(const std::string&, Callback callback = nullptr);

private:
    sqlite3* m_sqlite{nullptr};
    static std::shared_ptr<DBConnection> m_dbConn;
};