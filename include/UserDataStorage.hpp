#pragma once

#include <unordered_map>
#include <string>
#include <optional>

class UserDataStorage
{
public:
    void add_score(const std::string& user_id, size_t score);
    void remove_score(const std::string& user_id, size_t score);
    const std::optional<size_t> get_score(const std::string& user_id);

private:
    const std::optional<const std::string> update_database(const std::string& user_id);

private:
    std::unordered_map<std::string, size_t> m_user_score;
};