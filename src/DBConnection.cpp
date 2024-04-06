#include "DBConnection.hpp"
#include "DBTables.hpp"

#include <stdlib.h>

std::shared_ptr<DBConnection> DBConnection::m_dbConn = nullptr;

std::optional<const std::string> DBConnection::create_token_table()
{
    const std::string query = "CREATE TABLE IF NOT EXISTS token("\
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

std::optional<const std::string> DBConnection::insert_token(const Token& token)
{
    std::string query = std::format("INSERT INTO token(STATUS, ID, NAME, ACTIVATION_KEY, PRICE, PRIORITY)" \
        "VALUES ({}, \"{}\", \"{}\", \"{}\", {}, {})",
        token.status,
        token.id,
        token.name,
        token.key,
        token.price,
        token.priority
    );

    return execute_query(query);
}

std::optional<const std::string> DBConnection::insert_tokens(const std::unordered_map<std::string, Token>& tokens)
{
    std::string values;
    for (auto [id, token] : tokens) {
        if (!values.empty()) {
            values += ",";
        }
        values += std::format("({}, \"{}\", \"{}\", \"{}\", {}, {})",
            token.status,
            token.id,
            token.name,
            token.key,
            token.price,
            token.priority
        );
    } 
    return execute_query(std::format("INSERT INTO token(STATUS, ID, NAME, ACTIVATION_KEY, PRICE, PRIORITY) VALUES {};", values));
}

std::optional<const std::string> DBConnection::insert_user(const std::string& user_id)
{
    std::string query = std::format("INSERT INTO user_score(ID, SCORE) VALUES(\"{}\", {});",
        user_id,
        0
    );

    return execute_query(query);
}

std::optional<const std::string> DBConnection::update_token(const Token& token)
{
    std::string query = std::format(
        "UPDATE token SET " \
            "STATUS = {0},"
            "ID = \"{1}\"," \
            "NAME = \"{2}\"," \
            "ACTIVATION_KEY = \"{3}\"," \
            "PRICE = {4}," \
            "PRIORITY = {5} " \
        "WHERE ID = \"{1}\";", \
            token.status,
            token.id, 
            token.name, 
            token.key, 
            token.price, 
            token.priority
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

std::unordered_map<std::string, Token> DBConnection::select_tokens(const std::string& condition)
{  
    std::unordered_map<std::string, Token> tokens; 
    
    std::string query;
    if (!condition.empty()) {
        query = std::format("SELECT * FROM token WHERE {};", condition);
    } else {
        query = "SELECT * FROM token;";
    }

    sqlite3_stmt* stmt = nullptr;
    auto error_code = sqlite3_prepare_v2(m_sqlite, query.c_str(), -1, &stmt, nullptr);
    if (error_code != SQLITE_OK) {
        spdlog::error(std::format("select_tokens: {}", sqlite3_errmsg(m_sqlite)));
        return tokens;
    }

    while(true) {
        int error_code = sqlite3_step(stmt);
        if (error_code == SQLITE_ERROR) {
            spdlog::error(std::format("select_tokens: {}", sqlite3_errmsg(m_sqlite)));
            return tokens;
        }

        if (error_code == SQLITE_DONE) {
            return tokens;
        }

        Token token;
        token.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, DB::Token::ID));
        token.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, DB::Token::NAME));
        token.key = reinterpret_cast<const char*>(sqlite3_column_text(stmt, DB::Token::KEY));
        token.price = sqlite3_column_double(stmt, DB::Token::PRICE);
        token.priority = sqlite3_column_int(stmt, DB::Token::PRIORITY);

        tokens[token.id] = token;
    }

    return tokens;
}

std::optional<size_t> DBConnection::select_user_score(const std::string& user_id)
{
    std::string query = std::format("SELECT * FROM user_score WHERE ID = \"{}\"", user_id);
    
    sqlite3_stmt* stmt = nullptr;
    auto error_code = sqlite3_prepare_v2(m_sqlite, query.c_str(), -1, &stmt, nullptr);
    if (error_code != SQLITE_OK) {
        spdlog::error(std::format("select_user_score: {}", sqlite3_errmsg(m_sqlite)));
        return std::nullopt;
    }

    error_code = sqlite3_step(stmt);
    if (error_code == SQLITE_ERROR) {
        spdlog::error(std::format("select_user_score: {}", sqlite3_errmsg(m_sqlite)));
        return std::nullopt;
    }

    return sqlite3_column_int(stmt, DB::UserScore::SCORE);
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