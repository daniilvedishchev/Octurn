#pragma once

#include <string>
#include <unordered_map>

#include "dsl/fields/fill/fill.hpp"

inline const std::unordered_map<std::string,fill> fillByName = {
    {"fok", fill::fok},
    {"gtc", fill::gtc}
};