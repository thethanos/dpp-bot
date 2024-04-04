#pragma once

namespace DB {

enum Token
{
    STATUS = 0,
    ID,
    NAME,
    KEY,
    PRICE,
    PRIORITY,
    COLUMN_COUNT //not used in the actual table
};

};