#include "executionEngine.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <stdexcept>

#include "config/slippageTable.hpp"

executionEngine::executionEngine(std::unordered_map<std::string, AnyValue>& data, config& cfg)
    : data_(data), cfg_(cfg) {
        cfg_.slippage = getSlippageParams(cfg_,slippageTable);
    }

double executionEngine::getValue(const std::string& key, size_t idx){
    auto it = data_.find(key);
    if (it == data_.end())
        throw std::runtime_error(std::format("Series {} not found", key));

    const auto& series = std::get<std::vector<double>>(it->second);

    if (idx >= series.size())
        throw std::runtime_error("Index out of bounds");

    return series[idx];
}

Bar executionEngine::getBar(const std::string& ticker, size_t idx) {
    return {
        .open   = getValue(makeField(ticker, "open"), idx),
        .high   = getValue(makeField(ticker, "high"), idx),
        .low    = getValue(makeField(ticker, "low"), idx),
        .close  = getValue(makeField(ticker, "close"), idx),
        .volume = getValue(makeField(ticker, "volume"), idx)
    };
}

std::string executionEngine::makeField(const std::string& ticker, const std::string& field)
{
    return ticker + "_" + field;
}

double executionEngine::bpsToFrac(double bps) const {
    return bps / 10000.0;
}

SlippageParams executionEngine::getSlippageParams(const config& cfg,
    const std::unordered_map<Slippage,std::unordered_map<std::string,double>>& slippageTable){

    auto it = slippageTable.find(cfg.slippageRegime);

    if (it == slippageTable.end()) {
        throw std::runtime_error("Unknown slippageRegime");
    }

    return {
        it->second.at("max_participation"),
        it->second.at("impact_coef")
    };
}

void executionEngine::fillPosition(trade& trade){

}

void executionEngine::baseTradeInit(trade& trade){

    const Bar bar = getBar(trade.ticker,trade.timestamp.entryIdx);
    const double open = bar.open;
    const double bps  = bpsToFrac(cfg_.stopLossBps);

    const double slMult = (trade.type == ordertype::Buy) ? (1.0 - bps) : (1.0 + bps);
    trade.price.stopLossPrice = open * slMult;

    const double risk = cfg_.riskPerTrade * cfg_.equity * 0.01;
    const double dist = std::abs(open - trade.price.stopLossPrice);

    if (dist <= 0.0) {
        throw std::runtime_error("Stop-loss distance is zero");
    }

    trade.qty.targetQty = risk / dist;
    trade.isPending = (trade.qty.remainingQty() > 0.0);

}

double executionEngine::getAdjPrice(trade& trade,double const & open,double const& impactBps){
    double price{0.0};

    if (trade.type == ordertype::Sell){
        price = open*(1.0 - bpsToFrac(cfg_.spread) - bpsToFrac(impactBps));
    } else price = open*(1.0 + bpsToFrac(cfg_.spread) + bpsToFrac(impactBps));

    return price;
}

void executionEngine::stopLoss(trade& trade, double entryPrice) {
    const double bps = bpsToFrac(cfg_.stopLossBps);

    if (trade.type == ordertype::Buy) {
        trade.price.stopLossPrice = entryPrice * (1.0 - bps);
    } else {
        trade.price.stopLossPrice = entryPrice * (1.0 + bps);
    }
}

double executionEngine::calcImpactBps(double qty, double volume) const {
    if (volume <= 0.0 || qty <= 0.0) {
        return 0.0;
    }

    const double participation =
        std::min(qty / volume, cfg_.slippage.maxParticipation);

    return cfg_.slippage.impactCoef * std::sqrt(participation);
}

double executionEngine::calcQtyCash(double price) const {
    return cfg_.equity / price;
}

bool executionEngine::FOK(trade& trade) {
    const Bar bar = getBar(trade.ticker, trade.timestamp.entryIdx);
    const double needQty = trade.qty.targetQty;

    if (bar.volume <= 0.0) {
        throw std::runtime_error("Volume must be > 0");
    }

    if (needQty <= 0.0) {
        throw std::runtime_error("Target qty must be > 0");
    }

    const double qtyLiq = cfg_.slippage.maxParticipation * bar.volume;
    const double impactBps = calcImpactBps(needQty, bar.volume);
    const double price = getAdjPrice(trade, bar.open, impactBps);
    const double qtyCash = cfg_.equity / price;

    const bool canFillByLiquidity = (needQty <= qtyLiq);
    const bool canFillByCash = (needQty <= qtyCash);

    if (!canFillByLiquidity || !canFillByCash) {
        return false;
    }

    trade.qty.filledQty = needQty;
    trade.price.avgPrice = price;
    trade.isPending = false;

    stopLoss(trade, price);

    return true;
}

/**
------------------------- Good till Cancelled ----------------------
 @brief This function serves to fill market order candle by candle.
        If the order is too big to execute without significant price change,
        fill will be done in the following candles  in order to get a good avgPrice
*/
void executionEngine::executeGTCBar(trade& trade,size_t idx){

    const Bar bar = getBar(trade.ticker,idx);

    const double open = bar.open;
    const double volume = bar.volume;

    if (volume <= 0) return;

    double qtyLiq = cfg_.slippage.maxParticipation*bar.volume;
    double qty = std::min(qtyLiq,trade.qty.remainingQty());

    double price = getAdjPrice(trade, open, calcImpactBps(qty,volume));

    double qtyCash = calcQtyCash(price);

    qty = std::min(qty,qtyCash);

    if (qty <= 0.0) {
        return;
    }

    trade.qty.filledQty+=qty;
    trade.executionPrice.push_back({price,qty});

    if (trade.qty.remainingQty() == 0){
        trade.isPending=false;
    }
}

void executionEngine::averageExecutionPrice(trade& trade) const {
    const auto& fills = trade.executionPrice;

    double totalQty{0};
    double totalNotional{0};

    for (const auto& fill : fills){
        totalNotional+=fill.price*fill.qty;
        totalQty += fill.qty;
    }

    if (totalQty <= 0) {
        throw std::runtime_error("division by zero, filled qty is null");
    }

    trade.price.avgPrice = totalNotional/totalQty;
}
