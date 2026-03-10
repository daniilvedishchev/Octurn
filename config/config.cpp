#include "config/config.hpp"

config::config(std::unordered_map<std::string,AnyValue>* variables):variables_(variables){
    cfgValidator_ = configValidator();
    cfgValidator_.requiredCfgParametersCheckThenBuild(cfgRules,variables_,*this);
};