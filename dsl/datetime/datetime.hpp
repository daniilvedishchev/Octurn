#pragma once
#include <string>

struct datetime {
    std::string day;
    std::string month;
    std::string year;

    uint32_t timestamp;
    
    void toTimestamp();
};