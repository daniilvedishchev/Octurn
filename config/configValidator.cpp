#include <iostream>

#include "config/configValidator.hpp"

/**
    @brief This script is used for config validaton: requiredParams check && their characteristics

    @param 
        - cfgTmpIt -> an iterator of configParameter:Rule map
            -- first: parameter name, ex: "equity"
            -- second: rule structure that dictates constraints for parameter 
        - varIt -> iterator to an map element
            -- if exist && required -> build
            -- if doesn't exist && required -> throw
            -- else build 
        
**/

void configValidator::requiredCfgParametersCheckThenBuild(std::unordered_map<std::string, Rule>& rules,config& cfg){
    for (auto cfgTmpIt = rules.begin(); cfgTmpIt != rules.end();cfgTmpIt++){
        auto varIt = cfg.variables_->find(cfgTmpIt->first);
        if (cfgTmpIt->second.required == true && varIt == cfg.variables_->end()){
            std::runtime_error(std::format("Required config parameter \"{}\" is missing",cfgTmpIt->first));
        } else if (cfgTmpIt->second.required == false && varIt == cfg.variables_->end()){
            continue;
        } else build_config(cfgTmpIt,varIt,cfg);
    }
}

void configValidator::build_config(std::unordered_map<std::string, Rule>::iterator& cfgTmpIt,
                                std::unordered_map<std::string, Octurn::AnyValue>::iterator& varIt, config& cfg){
    std::string error;
    bool validated {(varIt == cfg.variables_->end()) ? cfgTmpIt->second.validate(varIt->second,cfg,error): cfgTmpIt->second.validate(cfgTmpIt->second.defaultValue.value(),cfg,error)};
    if (!validated){
        std::runtime_error(std::format("Unable to build config, error occured: {}",error));
    }
}