#pragma once

#include "Util.hpp"

enum Status
{
    INACTIVE = 0,
    ACTIVE
};

enum Columns
{
    STATUS = 0,
    ID,
    NAME,
    KEY,
    PRICE,
    PRIORITY
};

struct Game
{   
    size_t status;
    IdType id;
    std::string name;
    std::string key;
    double price;
    size_t priority;
};