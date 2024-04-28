#pragma once

#include "Util.hpp"
#include "Config.hpp"

#include <hiredis/hiredis.h>

#include <format>
#include <memory>

class UserDataStorageRedis
{
public:
    UserDataStorageRedis(const Config& config)
    {
        m_redis = std::unique_ptr<redisContext>(redisConnect(config.redis_host.c_str(), config.redis_port));
        if (m_redis == nullptr) {
            throw std::runtime_error("Failed to create Redis stream. Returned empty context.");
        }
        if (m_redis->err) {
            throw std::runtime_error(std::format("Failed to create Redis stream: {}", m_redis->err));
        }
    }

public:
    void add_score(const IdType& user_id, size_t score);
    void remove_score(const IdType& user_id, size_t score);
    void remove_score_counter(const IdType& user_id);
    const std::optional<size_t> get_score(const IdType& user_id);

private:
    std::unique_ptr<redisContext> m_redis;;
};