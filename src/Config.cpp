#include "Config.hpp"

bool Config::load(const std::string& path) 
{
    try {
        auto config = toml::parse_file(path);

        auto token = config["bot"]["token"].value<std::string>();
        if (!token.has_value()) {
            spdlog::error("Failed to load bot token");
            return false;
        }

        auto path_to_keys = config["game"]["path_to_keys"].value<std::string>();
        if (!path_to_keys.has_value()) {
            spdlog::error("Failed to load path to keys");
            return false;
        }

        auto psql_host = config["postgres"]["host"].value<std::string>();
        if (!psql_host.has_value()) {
            spdlog::error("Failed to load postgres host");
            return false;
        }

        auto psql_port = config["postgres"]["port"].value<int>();
        if (!psql_port.has_value()) {
            spdlog::error("Failed to load postgres port");
            return false;
        }

        auto psql_user = config["postgres"]["user"].value<std::string>();
        if (!psql_user.has_value()) {
            spdlog::error("Failed to load postgres user");
            return false;
        }

        auto psql_password = config["postgres"]["password"].value<std::string>();
        if (!psql_password.has_value()) {
            spdlog::error("Failed to load postgres password");
            return false;
        }

        auto psql_dbname = config["postgres"]["dbname"].value<std::string>();
        if (!psql_dbname.has_value()) {
            spdlog::error("Failed to load postgres db name");
            return false;
        }

        auto redis_host = config["redis"]["host"].value<std::string>();
        if (!redis_host.has_value()) {
            spdlog::error("Failed to load redis host");
            return false;
        }

        auto redis_port = config["redis"]["port"].value<int>();
        if (!redis_port.has_value()) {
            spdlog::error("Failed to load redis port");
            return false;
        }

        auto game_price = config["economy"]["game_price"].value<int>();
        if (!game_price.has_value()) {
            spdlog::error("Failed to load game price");
            return false;
        }

        this->token = token.value();
        this->path_to_keys = path_to_keys.value();
        this->psql_host = psql_host.value();
        this->psql_port = psql_port.value();
        this->psql_user = psql_user.value();
        this->psql_password = psql_password.value();
        this->psql_dbname = psql_dbname.value();
        this->redis_host = redis_host.value();
        this->redis_port = redis_port.value();
        this->game_price = game_price.value();
        print();
    }
    catch (const toml::parse_error& err) {
        spdlog::error("Failed to load config file: {}", path);
        return false;
    }
    return true;
}

void Config::print()
{
    spdlog::info("Configuration parsed successfully.");
    spdlog::info("Token: {}...", token.substr(0, token.size()/4));
    spdlog::info("Path to keys: {}", path_to_keys);
    spdlog::info("Postgres host: {}", psql_host);
    spdlog::info("Postgres port: {}", psql_port);
    spdlog::info("Postgres db name: {}", psql_dbname);
    spdlog::info("Redis host: {}", redis_host);
    spdlog::info("Redis port: {}", redis_port);
    spdlog::info("Game price: {}", game_price);
}