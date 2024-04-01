#include "TokenStorage.hpp"

const std::optional<const std::string> TokenStorage::load_from_file(const std::string& path) 
{   
    rapidcsv::Document doc(path, rapidcsv::LabelParams(-1, -1));
    if (doc.GetRowCount() == 0) {
        const std::string error = "The file is empty\n";
        std::cout << error;
        return error;
    }

    auto dbConn = DBConnection::get_conn();

    for (size_t row(0); row < doc.GetRowCount(); row++) 
    {
        auto status = doc.GetCell<size_t>(STATUS, row);
        if (status != ACTIVE) {
            continue;
        }

        Token token;
        token.id = doc.GetCell<std::string>(ID, row);
        token.name = doc.GetCell<std::string>(NAME, row);
        token.key = doc.GetCell<std::string>(KEY, row);
        token.price = doc.GetCell<double>(PRICE, row);
        token.priority = doc.GetCell<size_t>(PRIORITY, row);

        m_tokens[token.id] = token;
        if (auto error = dbConn->insert_token(token); error.has_value()) {
            return error;
        }
    }

    m_randomizer.load_ids(m_tokens);
    return std::nullopt;
}

const std::optional<const std::string> TokenStorage::load_from_db()
{
    return std::nullopt;
}

const std::optional<const std::string> TokenStorage::play(const std::string& user_id)
{
    const std::string id = m_randomizer.get_random_id();
    if (id.empty()) {
        return std::nullopt;
    }

    auto tokenIt = m_tokens.find(id);
    if (tokenIt == m_tokens.end()) {
        return std::nullopt;
    }
        
    m_winners[user_id] = tokenIt->second;
    return tokenIt->second.name;
}

const std::optional<Token> TokenStorage::get_prize(const std::string& user_id)
{
    auto prizeIt = m_winners.find(user_id);
    if (prizeIt == m_winners.end()) {
        return std::nullopt;
    }

    return prizeIt->second;
}