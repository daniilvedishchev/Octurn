#include "maps.hpp"

std::unordered_map<std::string, taFunctionCall> functionMap = {
    {"MA", [](const multiValue& args, std::unordered_map<std::string, AnyValue>& vars) -> std::vector<double> {
        return MA(args, vars);
    }},
    {"RSI", [](const multiValue& args, std::unordered_map<std::string, AnyValue>& vars) -> std::vector<double> {
        return RSI(args, vars);
    }}
};

std::unordered_map<uint8_t, std::string> OHLC_INDEX_MAP = {
    {0,"open"},
    {1,"high"},
    {2,"low"},
    {3,"close"},
};

