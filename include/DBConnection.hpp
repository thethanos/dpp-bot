#pragma once

#include "sqlite3.h"
#include <exception> 
#include <iostream>
#include <memory>
#include <format>
#include <optional>

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
            std::cout << "Data base connection opened successfully\n";
        }

        return m_dbConn;
    }

    std::optional<const std::string> create_table()
    {
        return std::nullopt;
    }

    std::optional<const std::string> insert()
    {
        return std::nullopt;
    }

private:
    sqlite3* m_sqlite{nullptr};
    static std::shared_ptr<DBConnection> m_dbConn;
};