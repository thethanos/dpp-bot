#pragma once

#include <dpp/dpp.h>
#include <spdlog/spdlog.h>
#include <dpp/nlohmann/json.hpp>

#include <optional>

using IdType = std::string;

struct ParentEvent
{
    std::string name;
    IdType user_id;
    IdType event_id;
};

struct EventMeta
{
    ParentEvent parent;
};

inline std::optional<EventMeta> get_event_meta(const dpp::interaction_create_t& event)
{
    EventMeta meta;
    try {
        auto raw_event = nlohmann::json::parse(event.raw_event);
        meta.parent.name = raw_event["d"]["message"]["interaction"]["name"];
        meta.parent.user_id = raw_event["d"]["message"]["interaction"]["user"]["id"];
        meta.parent.event_id = raw_event["d"]["message"]["id"];
    } catch (const std::exception& e) {
        spdlog::error(std::format("get_event_meta: {}", e.what()));
        return std::nullopt;
    }
    return meta;
}