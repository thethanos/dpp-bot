#pragma once

#include <string>

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

struct Token
{   
    std::string id;
    std::string name;
    std::string key;
    double price;
    size_t priority;
};