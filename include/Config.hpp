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
private:
    void print();
};