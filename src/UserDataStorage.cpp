#include "UserDataStorage.hpp"
#include "DBConnection.hpp"

void UserDataStorage::add_score(const std::string& user_id, size_t score)
{
    m_user_score[user_id]+= score;
    if ((m_user_score[user_id] % 10) == 1) {
        update_database(user_id);
    }
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
    return DBConnection::get_conn()->select_user_score(user_id);
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