#pragma once

#include "UserSession.hpp"
#include "Util.hpp"

#include <unordered_map>
#include <optional>

class UserDataStorage
{
public:
    const std::optional<const std::string> create_table();
    void add_score(const IdType& user_id, size_t score);
    void remove_score(const IdType& user_id, size_t score);
    const std::optional<size_t> get_score(const IdType& user_id);
    const std::optional<const std::string> add_win(const IdType& user_id, const IdType& event_id, const Game& game);
    const std::optional<Game> get_win(const IdType& user_id, const IdType& event_id);
    const int get_cursor(const IdType& user_id, const IdType& event_id, const std::string& direction);
    void set_cursor(const IdType& user_id, const IdType& event_id, int page_number);

private:
    const std::optional<const std::string> update_database(const IdType& user_id);

private:
    std::unordered_map<IdType, size_t> m_user_score;
    std::unordered_map<IdType, UserSession> m_user_sessions;
};