#include "TokenStorage.hpp"

#include <algorithm>

const std::optional<const std::string> TokenStorage::create_table()
{
    if (auto error = DBConnection::get_conn()->create_token_table(); error.has_value()) {
        return std::format("create_token_table: {}", error.value());
    }

    return std::nullopt;
}

const std::optional<const std::string> TokenStorage::load_from_file(const std::string& path) 
{   
    rapidcsv::Document doc(path, rapidcsv::LabelParams(-1, -1));
    if (doc.GetRowCount() == 0) {
        return std::format("GetRowCount: {}; The file is empty", path);
    }

    for (size_t row(0); row < doc.GetRowCount(); row++) 
    {
        auto status = doc.GetCell<size_t>(STATUS, row);
        if (status != ACTIVE) {
            continue;
        }

        Token token;
        token.status = status;
        token.id = doc.GetCell<std::string>(ID, row);
        token.name = doc.GetCell<std::string>(NAME, row);
        token.key = doc.GetCell<std::string>(KEY, row);
        token.price = doc.GetCell<double>(PRICE, row);
        token.priority = doc.GetCell<size_t>(PRIORITY, row);

        m_tokens[token.id] = token;
    }

    if (auto error = DBConnection::get_conn()->insert_tokens(m_tokens); error.has_value()) {
        return std::format("insert_tokens: {}", error.value());
    }

    m_randomizer.load_ids(m_tokens);
    return std::nullopt;
}

const std::optional<const std::string> TokenStorage::load_from_db()
{   
    update_available_games();
    m_randomizer.load_ids(m_tokens);
    return std::nullopt;
}

const std::optional<const Token> TokenStorage::play()
{
    auto id = m_randomizer.get_random_id();
    if (!id.has_value()) {
        return std::nullopt;
    }

    auto tokenIt = m_tokens.find(id.value());
    if (tokenIt == m_tokens.end()) {
        return std::nullopt;
    }

    return tokenIt->second;
}

const std::optional<const std::string> TokenStorage::deactivate(Token token)
{   
    token.status = INACTIVE;
    auto error = DBConnection::get_conn()->update_token(token);
    if (error.has_value()) {
        return std::format("update_token: {}", error.value());
    }

    update_available_games();
    spdlog::info("{} is deactivated and removed from random", token.name);
    return std::nullopt;
}

const std::optional<Token> TokenStorage::get_prize(const std::string& game_id)
{
    auto tokenIt = m_tokens.find(game_id);
    if (tokenIt == m_tokens.end()) {
        return std::nullopt;
    }

    return tokenIt->second;
}

const std::optional<const std::string> TokenStorage::get_list_page(const std::string& direction)
{
    std::lock_guard guard(m_mutex);
    if (direction == "next" && m_page_cursor < m_token_name_pages.size() - 1) {
        m_page_cursor++;
    }
    if (direction == "prev" && m_page_cursor > 0) {
        m_page_cursor--;
    }
    
    return m_token_name_pages[m_page_cursor];
}

const std::optional<const std::string> TokenStorage::update_available_games()
{
    m_tokens = DBConnection::get_conn()->select_tokens(std::format("WHERE STATUS = {} ORDER BY NAME", int(ACTIVE)));
    if (m_tokens.empty()) {
        return "select_tokens: failed to load tokens";
    }
    m_token_name_pages.clear();

    size_t counter = 0;
    std::string page;
    for (auto [id, token] : m_tokens) {
        if (counter == m_tokens.size() - 1) {
            m_token_name_pages.push_back(page);
            return std::nullopt;
        }
        if (counter > 0 && counter % 10 == 0) {
            m_token_name_pages.push_back(page);
            page.clear();
        }
        page += std::format("{}. {}\n", counter + 1, token.name);
        counter++;
    }

    return std::nullopt;
}