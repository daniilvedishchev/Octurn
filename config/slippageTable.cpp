#include <unordered_map>
#include "config/slippageTable.hpp"
#include "config/configTypes.hpp"

std::unordered_map<Slippage,std::unordered_map<std::string,double>> slippageTable {
    {Slippage::base,{
        {"max_participation",0.05},
        {"impact_coef",20.0},
        {"slippage",2.0}
    }       
    },
    {Slippage::optimistic,{
        {"max_participation",0.10},
        {"impact_coef",10.0},
        {"slippage",1.0}
    }
    },
    {Slippage::pessimistic,{
        {"max_participation",0.02},
        {"impact_coef",30.0},
        {"slippage",4.0}
    }}
};