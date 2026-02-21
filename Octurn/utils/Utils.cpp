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
