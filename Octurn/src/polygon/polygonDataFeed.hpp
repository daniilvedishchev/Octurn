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

        std::unordered_map<std::string, AnyValue> dataMapVec; 

        explicit polygonDataFeed(polygonClient&& client);
        void loadBars(const std::string& ticker,int multiplier,const std::string& from,const std::string& to,const std::string& timespan);
};
