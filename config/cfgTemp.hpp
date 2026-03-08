#pragma once
#include <functional>
#include <variant>
#include <string>
#include <optional>
#include <unordered_map>
#include "config/configStruc.hpp"
#include "types/types.hpp"

using Octurn::AnyValue;
enum class ValueType {Double,String,Mode};
enum class Slippage {optimistic,base,pessimistic};
//using Value = std::variant<std::string,bool,double,Mode>;

struct Rule {
    ValueType type;
    bool required;
    std::optional<AnyValue> defaultValue;
    std::function<bool(const AnyValue&,config& cfg,std::string&)> validate;
};

std::unordered_map<Slippage,std::unordered_map<std::string,double>> SlippageCfg;

extern std::unordered_map<std::string, Rule> cfgTemplate;