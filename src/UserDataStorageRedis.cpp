#include "UserDataStorageRedis.hpp"

#include <spdlog/spdlog.h>

void UserDataStorageRedis::add_score(const IdType& user_id, size_t score)
{
    auto reply = std::unique_ptr<redisReply>((redisReply*)redisCommand(m_redis.get(),"INCR %s", user_id.c_str()));
    if (reply->type == REDIS_REPLY_ERROR) {
        spdlog::error("add_score error for user: {}", user_id);
    }
}

void UserDataStorageRedis::remove_score(const IdType& user_id, size_t score)
{
    auto reply = std::unique_ptr<redisReply>((redisReply*)redisCommand(m_redis.get(),"DECRBY %s %d", user_id.c_str(), score));
    if (reply->type == REDIS_REPLY_ERROR) {
        spdlog::error("remove_score error for user: {}", user_id);
    }
}

void UserDataStorageRedis::remove_score_counter(const IdType& user_id)
{
    auto reply = std::unique_ptr<redisReply>((redisReply*)redisCommand(m_redis.get(),"DEL %s", user_id.c_str()));
    if (reply->type == REDIS_REPLY_ERROR) {
        spdlog::error("remove_score_counter error for user: {}", user_id);
    }
}

const std::optional<size_t> UserDataStorageRedis::get_score(const IdType& user_id)
{   
    auto reply = std::unique_ptr<redisReply>((redisReply*)redisCommand(m_redis.get(),"GET %s", user_id.c_str()));
    if (reply->type == REDIS_REPLY_ERROR) {
        spdlog::error("get_score error for user: {}", user_id);
        return 0;
    }
    return std::atoi(reply->str);
}