#include "UserDataStorage.hpp"

void UserDataStorage::add_score(const IdType& user_id, size_t score)
{
    m_redis->add_score(user_id, score);
}

void UserDataStorage::remove_score(const IdType& user_id, size_t score)
{
    m_redis->remove_score(user_id, score);
}

void UserDataStorage::remove_score_counter(const IdType& user_id)
{
    m_redis->remove_score_counter(user_id);
}

const std::optional<size_t> UserDataStorage::get_score(const IdType& user_id)
{
    return m_redis->get_score(user_id);
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