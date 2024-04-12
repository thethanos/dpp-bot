#pragma once

#include "Game.hpp"

#include <unordered_map>
#include <string>
#include <optional>

class UserSession
{
public:
    void add_game(const IdType& event_id, const Game& game) noexcept;
    std::optional<Game> get_game(const IdType& event_id);

    int get_cursor(const IdType& event_id, const std::string& direction);
    void set_cursor(const IdType& event_id, int page_number);

private:
    void advance_cursor(const IdType& event_id, const std::string& direction);

private:
    std::unordered_map<IdType, Game> m_wins;
    std::unordered_map<IdType, size_t> m_cursors;
};