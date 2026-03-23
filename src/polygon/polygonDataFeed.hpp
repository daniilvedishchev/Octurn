#pragma once
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include "polygonClient.hpp"
#include "types/types.hpp"

using Octurn::AnyValue;

class polygonDataFeed{
    private:
        polygonClient client_;
    public:
        explicit polygonDataFeed(polygonClient&& client);
        std::unordered_map<std::string, AnyValue> loadBars(const std::string& ticker,int multiplier,const std::string& from,const std::string& to,const std::string& timespan);
};
