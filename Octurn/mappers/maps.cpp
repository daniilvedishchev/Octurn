#include "maps.hpp"

std::unordered_map<std::string, taFunctionCall> functionMap = {
    {"MA", [](const multiValue& args,
              std::unordered_map<std::string, AnyValue>& vars) -> std::vector<double> {
        return MA(args,vars);
    }}
    // {"RSI", [](const multiValue& args, std::unordered_map<std::string, AnyValue>& data, std::unordered_map<std::string, AnyValue>& vars) -> std::vector<double> {
    //     return RSI(args, data, vars);
    // }}
};
