#include "TokenStorage.hpp"

const std::optional<const std::string> TokenStorage::load_from_file(const std::string& path) 
{   
    rapidcsv::Document doc(path, rapidcsv::LabelParams(-1, -1));
    if (doc.GetRowCount() == 0) {
        return std::format("load_from_file: {}; The file is empty", path);
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
        return std::format("load_from_file: {}", error.value());
    }

    m_randomizer.load_ids(m_tokens);
    return std::nullopt;
}

const std::optional<const std::string> TokenStorage::load_from_db()
{   
    auto tokens = DBConnection::get_conn()->select_tokens(std::format("STATUS = {}", int(ACTIVE)));
    if (tokens.empty()) {
        return "load_from_db: failed to load tokens";
    }

    m_tokens = std::move(tokens);
    m_randomizer.load_ids(m_tokens);
    return std::nullopt;
}

const std::optional<const std::string> TokenStorage::play(const std::string& user_id)
{
    auto id = m_randomizer.get_random_id();
    if (!id.has_value()) {
        return std::nullopt;
    }

    auto tokenIt = m_tokens.find(id.value());
    if (tokenIt == m_tokens.end()) {
        return std::nullopt;
    }
        
    m_winners[user_id] = tokenIt->second;
    return tokenIt->second.name;
}

const std::optional<const std::string> TokenStorage::deactivate(Token token)
{   
    token.status = INACTIVE;
    auto error = DBConnection::get_conn()->update_token(token);
    if (error.has_value()) {
        return std::format("deactivate: {}", error.value());
    }

    m_tokens.erase(token.id);
    spdlog::info("{} is deactivated and removed from random", token.name);
    return std::nullopt;
}

const std::optional<Token> TokenStorage::get_prize(const std::string& user_id)
{
    auto prizeIt = m_winners.find(user_id);
    if (prizeIt == m_winners.end()) {
        return std::nullopt;
    }

    return prizeIt->second;
}