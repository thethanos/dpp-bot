#pragma once

namespace DB {

enum Token
{
    ID = 0,
    NAME,
    KEY,
    PRICE,
    PRIORITY,
    COLUMN_COUNT //not used in the actual table
};

};