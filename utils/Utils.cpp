#include "Utils.hpp"
#include <stdexcept>
#include "types/types.hpp"

using Octurn::AnyValue;
using Octurn::multiValue;

// ===============================================
//           Converts string to boolean
// ===============================================
bool str_to_bool(const std::string& s) {
    if (s == "true" || s == "1") return true;
    if (s == "false" || s == "0") return false;
    throw std::invalid_argument("Invalid boolean string: " + s);
}

// ===============================================
//  Prints arrays of bools doubles or nested ones
// ===============================================
std::string print_any_value(AnyValue& value){
    std::string str = "[";
   
    return std::visit([&](const auto& val)->std::string {
        using T = std::decay_t<decltype(val)>;
        if constexpr(std::is_same_v<T,std::vector<bool>>){
            for (size_t i=0;i<val.size();i++){
                bool bl = val[i];
                str += bl ? "true" : "false";

                if (i+1<val.size()){
                    str+=",";
                }
            }
        }else if constexpr (std::is_same_v<T,std::vector<double>>){
            for (size_t i=0;i<val.size();i++){
                double flt = val[i];
                str += std::to_string(flt);

                if (i+1<val.size()){
                    str+=",";
                }
            }
        }   
        str+="]";
        return str;
    },value);
}

void printVariables(Interpreter& interp){
    std::cout << "\nVariables:\n";
    for (auto& [key, value] : interp.get_variables()) {
        if (std::holds_alternative<std::vector<bool>>(value) ||
            std::holds_alternative<std::vector<double>>(value)) {
            auto str = print_any_value(value);
            std::cout << "  " << key << " = " << str << "\n";
        } else if (std::holds_alternative<bool>(value)) {
            std::cout << "  " << key << " = " << std::get<bool>(value) << "\n";
        } else if (std::holds_alternative<double>(value)) {
            std::cout << "  " << key << " = " << std::get<double>(value) << "\n";
        } else if (std::holds_alternative<std::string>(value)) {
            std::cout << "  " << key << " = " << std::get<std::string>(value) << "\n";
        }
    }

    std::cout << "\nData:\n";
    for (auto& [key, value] : interp.get_data()) {
        if (std::holds_alternative<std::vector<bool>>(value) ||
            std::holds_alternative<std::vector<double>>(value)) {
            auto str = print_any_value(value);
            std::cout << "  " << key << " = " << str << "\n";
        }
    }

    std::cout << "\n Flags:\n";
    for (const auto& [key, value] : interp.get_flags()) {
        std::cout << "  " << key << " = " << value << "\n";
    }
}