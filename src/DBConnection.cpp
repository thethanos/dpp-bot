#include "DBConnection.hpp"
#include "DBTables.hpp"

#include <stdlib.h>

std::shared_ptr<DBConnection> DBConnection::m_dbConn = nullptr;

std::optional<const std::string> DBConnection::create_token_table()
{
    const std::string query = "CREATE TABLE IF NOT EXISTS token("\
        "ID CHAR(20) PRIMARY KEY NOT NULL,"\
        "NAME CHAR(30) NOT NULL,"\
        "ACTIVATION_KEY CHAR(30) NOT NULL," \
        "PRICE REAL NOT NULL," \
        "PRIORITY INT NOT NULL" \
    ");";

    return execute_query(query);
}

std::optional<const std::string> DBConnection::insert_token(const Token& token)
{
    std::string query = std::format("INSERT INTO token(ID, NAME, ACTIVATION_KEY, PRICE, PRIORITY)" \
        "VALUES (\"{}\", \"{}\", \"{}\", {}, {})",
        token.id,
        token.name,
        token.key,
        token.price,
        token.priority
    );

    return execute_query(query);
}

std::unordered_map<std::string, Token> DBConnection::select_tokens(const std::string& condition)
{  
    std::unordered_map<std::string, Token> tokens; 
    
    std::string query;
    if (!condition.empty()) {
        query = std::format("SELECT * FROM token WHERE {};", condition);
    } else {
        query = "SELECT * FROM token;";
    }

    sqlite3_stmt* stmt;
    auto error_code = sqlite3_prepare_v2(m_sqlite, query.c_str(), -1, &stmt, nullptr);
    if (error_code != SQLITE_OK) {
        spdlog::error(sqlite3_errmsg(m_sqlite));
        return tokens;
    }

    while(true) {
        int error_code = sqlite3_step(stmt);
        if (error_code == SQLITE_ERROR) {
            spdlog::error(sqlite3_errmsg(m_sqlite));
            return tokens;
        }

        if (error_code == SQLITE_DONE) {
            return tokens;
        }

        Token token;
        token.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, DB::ID));
        token.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, DB::NAME));
        token.key = reinterpret_cast<const char*>(sqlite3_column_text(stmt, DB::KEY));
        token.price = sqlite3_column_double(stmt, DB::PRICE);
        token.priority = sqlite3_column_int(stmt, DB::PRIORITY);

        tokens[token.id] = token;
    }

    return tokens;
}

std::optional<const std::string> DBConnection::execute_query(const std::string& query, Callback callback)
{
    spdlog::info("Executing query: {}", query);

    int error_code = sqlite3_exec(m_sqlite, query.c_str(), callback, nullptr, nullptr);
    if (error_code != SQLITE_OK) {
        spdlog::error(sqlite3_errmsg(m_sqlite));
        return sqlite3_errmsg(m_sqlite);
    }

    spdlog::info("Query executed successfully!");
    return std::nullopt;
}