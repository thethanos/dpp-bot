#include "UserDataStorage.hpp"
#include "DBConnection.hpp"

const std::optional<const std::string> UserDataStorage::create_table()
{
    if (auto error = m_dbConn->create_user_score_table(); error.has_value()) {
        return std::format("create_user_score_table: {}", error.value());
    }

    return std::nullopt;
}

void UserDataStorage::add_score(const IdType& user_id, size_t score)
{
    m_user_score[user_id]+= score;
}

void UserDataStorage::remove_score(const IdType& user_id, size_t score)
{
    if (m_user_score[user_id] == 0 || int(m_user_score[user_id] - score) <= 0) {
        m_user_score[user_id] = 0;
    } else {
        m_user_score[user_id] -= score;
    }

    update_database(user_id);
}

const std::optional<size_t> UserDataStorage::get_score(const IdType& user_id)
{
    if (auto error = update_database(user_id); error.has_value()) {
        spdlog::error("update_database : {}", error.value());
        return std::nullopt;
    }
    return m_user_score[user_id];
}

const std::optional<const std::string> UserDataStorage::add_win(const IdType& user_id, const IdType& event_id, const Game& game)
{
    if (!m_user_sessions.contains(user_id)) {
        m_user_sessions[user_id] = UserSession{};
    }

    m_user_sessions[user_id].add_game(event_id, game);
    return std::nullopt;
}

const std::optional<Game> UserDataStorage::get_win(const IdType& user_id, const IdType& event_id)
{
    if (!m_user_sessions.contains(user_id)) {
        return std::nullopt;
    }

    return m_user_sessions[user_id].get_game(event_id);
}

const int UserDataStorage::get_cursor(const IdType& user_id, const IdType& event_id, const std::string& direction)
{
    if (!m_user_sessions.contains(user_id)) {
        m_user_sessions[user_id] = UserSession{};
    }

    return m_user_sessions[user_id].get_cursor(event_id, direction);
}

void UserDataStorage::set_cursor(const IdType& user_id, const IdType& event_id, int page_number)
{
    if (!m_user_sessions.contains(user_id)) {
        m_user_sessions[user_id] = UserSession{};
    }

    m_user_sessions[user_id].set_cursor(event_id, page_number);
}

const std::optional<const std::string> UserDataStorage::update_database(const IdType& user_id)
{
    auto current_score = m_dbConn->select_user_score(user_id);
    if (!current_score.has_value()) {
        if (auto error = m_dbConn->insert_user(user_id); error.has_value()) {
            return error;
        }
    }

    return m_dbConn->update_user_score(user_id, m_user_score[user_id]);
}