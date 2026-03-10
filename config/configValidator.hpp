#pragma once
#include <unordered_map>
#include "config/configRules.hpp"

class configValidator{
    public:
        void requiredCfgParametersCheckThenBuild(std::unordered_map<std::string, Rule>& rules,
                                            std::unordered_map<std::string,AnyValue>* variables,
                                            config& cfg);
        void configValidator::build_config(std::unordered_map<std::string, Rule>::iterator& cfgIt,
                                            std::unordered_map<std::string, Octurn::AnyValue>::iterator& varIt,
                                            std::unordered_map<std::string,AnyValue>* variables,config& cfg);
};