#include "MarketDataView.hpp"

#include <format>
#include <stdexcept>
#include <utility>

#include "node/Node.hpp"
#include "src/polygon/polygonClient.hpp"

using Octurn::AnyValue;

MarketDataView::MarketDataView(const std::string& apiKey) : feeder_(polygonClient(apiKey)) {}

MarketDataView::MarketDataView(polygonDataFeed&& feeder) : feeder_(std::move(feeder)) {}

std::string MarketDataView::makeField(const std::string& ticker, const std::string& field) {
    return ticker + "_" + field;
}

Bar MarketDataView::getBar(const std::string& ticker, size_t idx) {
    return {
        .open   = getValue(makeField(ticker, "open"), idx),
        .high   = getValue(makeField(ticker, "high"), idx),
        .low    = getValue(makeField(ticker, "low"), idx),
        .close  = getValue(makeField(ticker, "close"), idx),
        .volume = getValue(makeField(ticker, "volume"), idx)
    };
}

void MarketDataView::extract(const std::shared_ptr<ASTList>& list) {
    if (!list) {
        return;
    }

    for (auto& data_block : list->list) {
        std::string ticker, timespan, from, to;
        int multiplier = 0;

        if (auto fetching_params_node = std::dynamic_pointer_cast<ASTBlock>(data_block)) {
            auto& fetching_params = fetching_params_node->entries;
            for (auto& [key, value] : fetching_params) {
                if (auto value_node = std::dynamic_pointer_cast<ASTValueNode>(value)) {
                    if (std::holds_alternative<std::string>(value_node->value)) {
                        std::string strVal = std::get<std::string>(value_node->value);
                        if (key == "ticker") ticker = strVal;
                        else if (key == "from") from = strVal;
                        else if (key == "to") to = strVal;
                        else if (key == "timespan") timespan = strVal;
                    } else if (std::holds_alternative<double>(value_node->value)) {
                        if (key == "multiplier") {
                            multiplier = static_cast<int>(std::get<double>(value_node->value));
                        }
                    }
                }
            }
        }

        if (ticker.empty()) {
            throw std::runtime_error("No ticker found");
        }

        auto fetched = feeder_.loadBars(ticker, multiplier, from, to, timespan);
        dataMap_.merge(std::move(fetched));
    }
}

std::unordered_map<std::string, AnyValue>& MarketDataView::data() {
    return dataMap_;
}

const std::unordered_map<std::string, AnyValue>& MarketDataView::data() const {
    return dataMap_;
}

double MarketDataView::getValue(const std::string& key, size_t idx) const {
    auto it = dataMap_.find(key);
    if (it == dataMap_.end()) {
        throw std::runtime_error(std::format("Series {} not found", key));
    }

    const auto& series = std::get<std::vector<double>>(it->second);

    if (idx >= series.size()) {
        throw std::runtime_error("Index out of bounds");
    }

    return series[idx];
}
