#pragma once

#include "toml.hpp"
#include "spdlog/spdlog.h"

#include <memory>

class Config
{
public:
    Config(const std::string& path)
    {

    }

public:
    std::string token;
private:
    void print();
};