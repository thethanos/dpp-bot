#pragma once

#include "Util.hpp"

#include <dpp/dpp.h>
#include <spdlog/spdlog.h>
#include <dpp/nlohmann/json.hpp>

#include <optional>

struct ParentEvent
{
    std::string name;
    IdType user_id;
    IdType event_id;
};

struct EventMeta
{
    ParentEvent parent;
    IdType user_id;
    IdType event_id;
};

inline std::optional<EventMeta> get_event_meta(const dpp::interaction_create_t& event)
{
    EventMeta meta;
    try {
        auto raw_event = nlohmann::json::parse(event.raw_event);
        if (raw_event["d"].contains("message")) {
            meta.parent.name = raw_event["d"]["message"]["interaction_metadata"]["name"];
            meta.parent.user_id = raw_event["d"]["message"]["interaction_metadata"]["user_id"];
            meta.parent.event_id = raw_event["d"]["message"]["interaction_metadata"]["id"];
        }
        meta.user_id = raw_event["d"]["member"]["user"]["id"];
        meta.event_id = raw_event["d"]["id"];
    } catch (const std::exception& e) {
        spdlog::error(std::format("get_event_meta: {}", e.what()));
        return std::nullopt;
    }
    return meta;
}