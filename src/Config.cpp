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

        this->token = token.value();
        this->path_to_keys = path_to_keys.value();
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
}