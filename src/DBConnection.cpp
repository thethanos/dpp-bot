#include "DBConnection.hpp"

std::shared_ptr<DBConnection> DBConnection::m_dbConn = nullptr;

std::optional<const std::string> DBConnection::create_tokens_table()
{
    const std::string query = "CREATE TABLE IF NOT EXISTS tokens("\
        "ID CHAR(20) PRIMARY KEY NOT NULL,"\
        "NAME CHAR(30) NOT NULL,"\
        "ACTIVATION_KEY CHAR(30) NOT NULL," \
        "PRICE REAL NOT NULL," \
        "PRIORITY INT NOT NULL" \
    ");";

    return execute_query(query);
}

std::optional<const std::string> DBConnection::insert_token(const Token& token)
{
    std::string query = std::format("INSERT INTO tokens(ID, NAME, ACTIVATION_KEY, PRICE, PRIORITY)" \
        "VALUES (\"{}\", \"{}\", \"{}\", {}, {})",
        token.id,
        token.name,
        token.key,
        token.price,
        token.priority
    );

    return execute_query(query);
}

std::optional<const std::string> DBConnection::execute_query(const std::string& query, 
    const std::function<Callback>& callback)
{
    std::cout << std::format("Executing query: {}", query) << '\n';
    
    auto callback_c = callback.target<Callback>();

    int error_code = sqlite3_exec(m_sqlite, query.c_str(), callback_c, nullptr, nullptr);
    if (error_code != SQLITE_OK) {
        std::cout << sqlite3_errmsg(m_sqlite) << '\n';
        return sqlite3_errmsg(m_sqlite);
    }

    std::cout << "Query executed successfully!\n";
    return std::nullopt;
}