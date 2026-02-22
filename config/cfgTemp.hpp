#include <functional>
#include <variant>
#include <unordered_map>
#include "configStruc.hpp"

enum class ValueType {Double,String,Mode};
using Value = std::variant<std::string,bool,double,Mode>;

struct Rule {
    ValueType type;
    bool required;
    std::optional<Value> defaultValue;
    std::function<bool(const Value&,config& cfg,std::string&)> validate;
};

std::unordered_map<std::string, Rule> cfgTemplate = {
    { "equity", {
        ValueType::Double, true, std::nullopt,
        [](const Value& v, config& cfg,std::string& err){
            double equity = std::get<double>(v);
            if (equity <= 0) { err = "equity value must be > 0"; return false; }
            cfg.equity = equity;
            return true;
        }
    }},
    { "commissionBps", {
        ValueType::Double, false, Value{0.0},
        [](const Value& v,config& cfg, std::string& err){
            double commissionBps = std::get<double>(v);
            if (commissionBps < 0 || commissionBps >= 1) { err = "commissionBps must be >= 0 and < 1"; return false; }
            cfg.commissionBps = commissionBps;
            return true;
        }
    }},
    { "slippageBps", {
        ValueType::Double, true, std::nullopt,
        [](const Value& v,config& cfg, std::string& err){
            double slippageBps = std::get<double>(v);
            if (slippageBps < 0) { err = "slippageBps must be >= 0"; return false; }
            cfg.slippageBps = slippageBps;
            return true;
        }
    }},
    { "positionMode", {
        ValueType::String, true, Value{Mode::percent},
        [](const Value& v,config& cfg, std::string& err){
            std::string d = std::get<std::string>(v);
            if (d == "fixed" || d == "percent") {
                if (d == "fixed") cfg.positionMode = Mode::fixed;
                else cfg.positionMode = Mode::percent;
                return true;}
            else {
                err = "positionMode must be either \"fixed\" or \"percent\"";
                return false;
            }
        }
    }}
};
