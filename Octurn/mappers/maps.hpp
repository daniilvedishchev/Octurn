#pragma once
#include "types.hpp" // Project namespace //
#include "lexer/Token.hpp"
#include <map>
#include <string>
#include "TA/taLib.hpp" // Technical analysis functions //

using Octurn::multiValue;
using Octurn::taFunctionCall;

// ================================================================== //
// Use mappings to execute functions, taking variables from 
// Interpreter class
// ================================================================== //

extern std::unordered_map<std::string,taFunctionCall> functionMap;
extern std::unordered_map<uint8_t, std::string> OHLC_INDEX_MAP;