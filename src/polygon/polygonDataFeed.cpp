#include "polygonDataFeed.hpp"
#include <iostream>

polygonDataFeed::polygonDataFeed(polygonClient&& client) : client_(std::move(client)) {};

void polygonDataFeed::loadBars(const std::string& ticker,int multiplier,const std::string& from,const std::string& to,const std::string& timespan){
    
    nlohmann::json json = client_.fetchData(ticker,multiplier,from,to,timespan);

    std::vector<double> open, high, low, close, volume, timestamp;

    auto n = json["results"].size();
    open.reserve(n);
    high.reserve(n);
    low.reserve(n);
    close.reserve(n);
    volume.reserve(n);
    timestamp.reserve(n);

    for (const auto& bar : json["results"]) {
        std::cout<<"Getting values"<<bar["o"]<<"\n";
        open.emplace_back(bar["o"].get<double>());
        high.emplace_back(bar["h"].get<double>());
        low.emplace_back(bar["l"].get<double>());
        close.emplace_back(bar["c"].get<double>());
        volume.emplace_back(bar["v"].get<double>());
        timestamp.emplace_back(static_cast<double>(bar["t"].get<int64_t>()));
    }

    const auto base = ticker + "_";
    dataMapVec[base + "open"] = AnyValue{std::move(open)};
    dataMapVec[base + "high"] = AnyValue{std::move(high)};
    dataMapVec[base + "low"] = AnyValue{std::move(low)};
    dataMapVec[base + "close"] = AnyValue{std::move(close)};
    dataMapVec[base + "volume"] = AnyValue{std::move(volume)};
    dataMapVec[base + "timestamp"] = AnyValue{std::move(timestamp)};
}
