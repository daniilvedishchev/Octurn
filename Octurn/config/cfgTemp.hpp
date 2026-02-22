#include <functional>
#include <variant>
#include <string>
#include <optional>
#include <unordered_map>
#include "configStruc.hpp"
#include "types/types.hpp"

using Octurn::AnyValue;
enum class ValueType {Double,String,Mode};
//using Value = std::variant<std::string,bool,double,Mode>;

struct Rule {
    ValueType type;
    bool required;
    std::optional<AnyValue> defaultValue;
    std::function<bool(const AnyValue&,config& cfg,std::string&)> validate;
};

std::unordered_map<std::string, Rule> cfgTemplate;