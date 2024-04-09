#pragma once

namespace DB {

namespace Game {
    enum 
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

namespace UserScore
{
    enum {
        ID = 0,
        SCORE,
        COLUMN_COUNT //not used in the actual table
    };
};

};