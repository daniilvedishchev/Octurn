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
    std::function<bool(const Value&,std::string&)> validate;
};

std::unordered_map<std::string, Rule> cfgTemplate = {
    { "equity", {
        ValueType::Double, true, std::nullopt,
        [](const Value& v, std::string& err){
            double d = std::get<double>(v);
            if (d <= 0) { err = "must be > 0"; return false; }
            return true;
        }
    }},
    { "commissionBps", {
        ValueType::Double, false, Value{0.0},
        [](const Value& v, std::string& err){
            double d = std::get<double>(v);
            if (d < 0 || d >= 1) { err = "must be >= 0 and < 1"; return false; }
            return true;
        }
    }},
    { "slippageBps", {
        ValueType::Double, true, std::nullopt,
        [](const Value& v, std::string& err){
            double d = std::get<double>(v);
            if (d < 0) { err = "must be >= 0"; return false; }
            return true;
        }
    }},
    { "positionMode", {
        ValueType::String, true, Value{Mode::percent},
        [](const Value& v, std::string& err){
            std::string d = std::get<std::string>(v);
            if (d == "fixed" || d == "percent") {return true;}
            else {
                err = "positionMode must be either \"fixed\" or \"percent\"";
                return false;
            }
        }
    }}
};
