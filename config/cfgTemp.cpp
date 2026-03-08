#include "config/cfgTemp.hpp"

std::unordered_map<std::string, Rule> cfgTemplate = {
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
            return true;
        }
    }},
    { "positionMode", {
        ValueType::String, false, AnyValue{std::string("percent")},
        [](const AnyValue& v,config& cfg, std::string& err){
            std::string positionMode = std::get<std::string>(v);
            if (positionMode == "fixed" || positionMode == "percent") {
                if (positionMode == "fixed") cfg.positionMode = Mode::fixed;
                else cfg.positionMode = Mode::percent;
                return true;
            }
            else {
                err = "positionMode must be either \"fixed\" or \"percent\"";
                return false;
            }
        }
    }},
    { "positionSize", {
        ValueType::Double, true, std::nullopt,
        [](const AnyValue& v,config& cfg, std::string& err){
            double positionSize = std::get<double>(v);
            if (cfg.positionMode == Mode::fixed) {
                if (positionSize<0) {
                    err ="fixed positionSize must be a positive number";
                    return false;
                };
                cfg.positionSize = positionSize;
                return true;
            } else {
                cfg.positionSize = cfg.equity * positionSize/100;
                return true;
            }
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
    }}
};
