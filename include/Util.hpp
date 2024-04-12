#pragma once

#include <string>

using IdType = std::string;

struct Page
{
    std::string content;
    size_t number;
};