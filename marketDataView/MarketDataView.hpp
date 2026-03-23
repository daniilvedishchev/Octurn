#pragma once
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>

#include "types/types.hpp"
#include "src/polygon/polygonDataFeed.hpp"

struct ASTList;

class MarketDataView {
private:
    polygonDataFeed feeder_;
    std::unordered_map<std::string, Octurn::AnyValue> dataMap_;

public:
    explicit MarketDataView(const std::string& apiKey);
    explicit MarketDataView(polygonDataFeed&& feeder);

    void extract(const std::shared_ptr<ASTList>& list);
    std::unordered_map<std::string, Octurn::AnyValue>& data();
    const std::unordered_map<std::string, Octurn::AnyValue>& data() const;
    double getValue(const std::string& key, size_t idx) const;
};
