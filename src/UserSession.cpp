#include "UserSession.hpp"

void UserSession::add_game(const IdType& event_id, const Game& game) noexcept
{
    m_wins[event_id] = game;
}

std::optional<Game> UserSession::get_game(const IdType& event_id)
{
    if (!m_wins.contains(event_id)) {
        return std::nullopt;
    }

    return m_wins[event_id];
}

int UserSession::get_cursor(const IdType& event_id, const std::string& direction)
{       
    if (!m_cursors.contains(event_id)) {
        m_cursors[event_id] = 0;
    }

    advance_cursor(event_id, direction);
    return m_cursors[event_id];
}

void UserSession::set_cursor(const IdType& event_id, int page_number)
{
    if (!m_cursors.contains(event_id)) {
        m_cursors[event_id] = 0;
    }

    m_cursors[event_id] = page_number;
}

void UserSession::advance_cursor(const IdType& event_id, const std::string& direction)
{
    if (direction == "next") {
        m_cursors[event_id]++;
    }

    if (direction == "prev") {
        if (m_cursors[event_id] == 0) {
            return;
        }

        m_cursors[event_id]--;
    }
}
