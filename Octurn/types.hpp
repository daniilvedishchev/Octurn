#pragma once
#include <variant>
#include <vector>
#include <string>
#include <map>

// ==== Global forward declaration ==== //
struct ASTNode;

namespace Octurn {
    // =================== Namespaces here ================== //
    struct AnyValue;
    using multiValue = std::vector<AnyValue>;
    
    using NodeMap = std::map<std::string, std::shared_ptr<ASTNode>>;
    using taFunctionCall = std::function<std::vector<double>(const multiValue& args,std::unordered_map<std::string,AnyValue>&variables_)>;

    struct AnyValue : std::variant<double, bool, std::string, multiValue, std::vector<double>,std::vector<bool>> {
        using base = std::variant<double, bool, std::string, multiValue, std::vector<double>,std::vector<bool>>;
        using base::base;
    };

}
