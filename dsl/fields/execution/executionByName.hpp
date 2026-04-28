#pragma once

#include <string>
#include <unordered_map>
#include "dsl/fields/execution/execution.hpp"

inline const std::unordered_map<std::string,execution> executionByName = {
    {"market", execution::market},
    {"limit", execution::limit}
};