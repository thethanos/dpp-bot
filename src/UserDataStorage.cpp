#include "UserDataStorage.hpp"
#include "DBConnection.hpp"

void UserDataStorage::add_score(const std::string& user_id, size_t score)
{
    m_user_score[user_id]+= score;
    if (m_user_score[user_id] >= 10) {
        update_database(user_id);
    }
}

const std::optional<size_t> UserDataStorage::get_score(const std::string& user_id)
{
    return std::nullopt;
}

const std::optional<const std::string> UserDataStorage::update_database(const std::string& user_id)
{
    return std::nullopt;
}