#include "UserDataStorageDB.hpp"
#include "DBTables.hpp"

#include "spdlog/spdlog.h"

#include <stdlib.h>

std::optional<const std::string> UserDataStorageDB::init_user_data_storage()
{
    const std::string query = "CREATE TABLE IF NOT EXISTS user_score(" \
        "ID CHAR(20) PRIMARY KEY NOT NULL," \
        "SCORE INT NOT NULL" \
    ");"; 

    return execute_simple_query(query);
}

std::optional<const std::string> UserDataStorageDB::insert_user(const std::string& user_id)
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

std::optional<const std::string> UserDataStorageDB::update_user_score(const std::string& user_id, size_t score)
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

std::optional<size_t> UserDataStorageDB::select_user_score(const std::string& user_id)
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

std::optional<const std::string> UserDataStorageDB::execute_simple_query(const std::string& query)
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