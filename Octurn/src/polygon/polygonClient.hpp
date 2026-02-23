#pragma once
#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

class polygonClient{
    private:
        std::string api_key;
    public:
        std::string ticker,from,to,timespan;
        polygonClient(const std::string& api_key);
        nlohmann::json fetchData(const std::string& ticker,int multiplier,const std::string& from,const std::string& to,const std::string& timespan);
};
