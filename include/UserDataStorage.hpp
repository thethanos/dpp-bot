#pragma once

#include "UserDataStorageRedis.hpp"
#include "UserSession.hpp"
#include "Util.hpp"

#include <unordered_map>
#include <optional>
#include <memory>

class UserDataStorage
{
public:
    UserDataStorage(std::unique_ptr<UserDataStorageRedis> redis)
        :m_redis(std::move(redis))
    {

    }

public:
    void add_score(const IdType& user_id, size_t score);
    void remove_score(const IdType& user_id, size_t score);
    void remove_score_counter(const IdType& user_id);
    const std::optional<size_t> get_score(const IdType& user_id);
    const std::optional<const std::string> add_win(const IdType& user_id, const IdType& event_id, const Game& game);
    const std::optional<Game> get_win(const IdType& user_id, const IdType& event_id);
    const int get_cursor(const IdType& user_id, const IdType& event_id, const std::string& direction);
    void set_cursor(const IdType& user_id, const IdType& event_id, int page_number);

private:
    std::unordered_map<IdType, UserSession> m_user_sessions;
    std::unique_ptr<UserDataStorageRedis> m_redis;
};