#include "maps.hpp"

std::unordered_map<std::string, taFunctionCall> functionMap = {
    {"MA", [](const multiValue& args, std::unordered_map<std::string, AnyValue>& vars, std::unordered_map<std::string, AnyValue>& data_) -> std::vector<double> {
        return MA(args,vars,data_);
    }},
    {"RSI", [](const multiValue& args, std::unordered_map<std::string, AnyValue>& vars,std::unordered_map<std::string, AnyValue>& data_) -> std::vector<double> {
        return RSI(args,vars,data_);
    }}
};
