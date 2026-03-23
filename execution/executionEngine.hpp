#pragma once

#include <string>
#include <unordered_map>

#include "config/config.hpp"
#include "trade/trade.hpp"
#include "types/types.hpp"
#include "account/account.hpp"

using Octurn::AnyValue;

class ExecutionEngine {
private:
    std::unordered_map<std::string, AnyValue>& data_;
    config& cfg_;
    account& account_;

    double bpsToFrac(double bps) const;
    SlippageParams getSlippageParams(const config& cfg,
        const std::unordered_map<Slippage,std::unordered_map<std::string,double>>& slippageTable);
    double getAdjPrice(trade& trade,double const & open,double const& impactBps);
    static std::string makeField(const std::string& ticker, const std::string& field);

    bool FOK(trade& trade);
    void stopLoss(trade& trade,double open);
    void averageExecutionPrice(trade& trade) const;
    Bar getBar(const std::string& ticker, size_t idx);
    double calcImpactBps(double qty, double volume) const;
    double calcQtyCash(const trade& trade, double price) const;
    void applyCashEffect(trade& trade, double qty, double price);

public:
    ExecutionEngine(std::unordered_map<std::string, AnyValue>& data, config& cfg, account& account);

    void initOrder(trade& trade);
    void executeGTCBar(trade& trade, size_t idx);
    void fillPosition(trade& trade);
    double getValue(const std::string& key, size_t idx);
};
