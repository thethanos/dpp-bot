#pragma once

#include "toml.hpp"
#include "spdlog/spdlog.h"

#include <memory>

class Config
{
public:
    bool load(const std::string& path);

public:
    std::string token;
    std::string path_to_keys;
    std::string psql_host;
    int psql_port;
    std::string psql_user;
    std::string psql_password;
    std::string psql_dbname;
private:
    void print();
};