#pragma once

#include <string>
#include <vector>

struct pendingOHLC{
    std::string ticker;
    int id;
};

inline std::vector<pendingOHLC> pending;
