#pragma once
#include <unordered_map>
#include "config/configTypes.hpp"

class config;

struct Rule {
    ValueType type;
    bool required;
    std::optional<AnyValue> defaultValue;
    std::function<bool(const AnyValue&,config& cfg,std::string&)> validate;
};

std::unordered_map<std::string, Rule> cfgRules;
