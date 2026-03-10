#pragma once
#include <functional>
#include <variant>
#include <string>
#include <optional>
#include <unordered_map>
#include "types/types.hpp"
#include "config/configUtilities.hpp"
#include "config/configValidator.hpp"
#include "config/configRules.hpp"
#include "config/slippageTable.hpp"


class config {
    public:

        double commissionBps;
        double equity; 
        double riskPerTrade;
        double stopLossBps;

        Slippage slippageRegime;

        config(std::unordered_map<std::string,AnyValue>* variables);
        
        configValidator cfgValidator_;
        std::unordered_map<std::string,AnyValue>* variables_;    
};
