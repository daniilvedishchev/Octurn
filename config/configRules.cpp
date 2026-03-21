#include "config/configRules.hpp"
#include "config/config.hpp"
#include "config/configUtilities.hpp"
std::unordered_map<std::string, Rule> cfgRules = {
    { "equity", {
        ValueType::Double, true, std::nullopt,
        [](const AnyValue& v, config& cfg,std::string& err){
            double equity = std::get<double>(v);
            if (equity <= 0) { err = "equity value must be > 0"; return false; }
            cfg.equity = equity;
            return true;
        }
    }},
    { "commissionBps", {
        ValueType::Double, false, AnyValue{0.0},
        [](const AnyValue& v,config& cfg, std::string& err){
            double commissionBps = std::get<double>(v);
            if (commissionBps < 0 || commissionBps >= 1) { err = "commissionBps must be >= 0 and < 1"; return false; }
            cfg.commissionBps = commissionBps;
            return true;
        }
    }},
    { "slippageRegime", {
        ValueType::Double, false, "base",
        [](const AnyValue& v,config& cfg, std::string& err){
            std::string slippage = std::get<std::string>(v);
            stringToSlippage(slippage,cfg);
            return true;
        }
    }},
    { "riskPerTrade", {
        ValueType::Double, true, std::nullopt,
        [](const AnyValue& v,config& cfg, std::string& err){
            double riskPerTrade = std::get<double>(v);
            if (riskPerTrade > 100 || riskPerTrade < 0){
                err = "riskPerTrade should be a positive value in the range of (0-100) %";
                return false;
            }
            return true;
        }    
    }},
    { "stopLossBps", {
        ValueType::Double, true, std::nullopt,
        [](const AnyValue& v,config& cfg, std::string& err){
            double stopLossBps = std::get<double>(v);
            if (stopLossBps < 0 || stopLossBps > 100){
                err = "stopLossBps should be a positive value in the range of (0-100)";
                return false;
            }
            return true;
        }    
    }}
};
