#include "UserDataStorage.hpp"
#include "DBConnection.hpp"

const std::optional<const std::string> UserDataStorage::create_table()
{
    if (auto error = DBConnection::get_conn()->create_user_score_table(); error.has_value()) {
        return std::format("create_user_score_table: {}", error.value());
    }
}

void UserDataStorage::add_score(const std::string& user_id, size_t score)
{
    m_user_score[user_id]+= score;
}

void UserDataStorage::remove_score(const std::string& user_id, size_t score)
{
    if (m_user_score[user_id] == 0 || int(m_user_score[user_id] - score) <= 0) {
        m_user_score[user_id] = 0;
    } else {
        m_user_score[user_id] -= score;
    }

    update_database(user_id);
}

const std::optional<size_t> UserDataStorage::get_score(const std::string& user_id)
{
    if (auto error = update_database(user_id); error.has_value()) {
        spdlog::error("update_database : {}", error.value());
        return std::nullopt;
    }
    return m_user_score[user_id];
}

const std::optional<const std::string> UserDataStorage::update_database(const std::string& user_id)
{
    auto dbConn = DBConnection::get_conn();

    auto current_score = dbConn->select_user_score(user_id);
    if (!current_score.has_value()) {
        if (auto error = dbConn->insert_user(user_id); error.has_value()) {
            return error;
        }
    }

    return dbConn->update_user_score(user_id, m_user_score[user_id]);
}