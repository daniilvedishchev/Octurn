#include "ExecutionEngine.hpp"

#include <cmath>
#include <format>
#include <stdexcept>
#include <algorithm>

#include "config/slippageTable.hpp"

ExecutionEngine::ExecutionEngine(std::unordered_map<std::string, AnyValue>& data, config& cfg,account& account)
    : data_(data), cfg_(cfg), account_(account) {
        cfg_.slippage = getSlippageParams(cfg_,slippageTable);
    }

double ExecutionEngine::getValue(const std::string& key, size_t idx){
    auto it = data_.find(key);
    if (it == data_.end()) throw std::runtime_error(std::format("Series {} not found", key));

    const auto& series = std::get<std::vector<double>>(it->second);

    if (idx >= series.size()){
        throw std::runtime_error("Index out of bounds");
    }

    return series[idx];
}

Bar ExecutionEngine::getBar(const std::string& ticker, size_t idx) {
    return {
        .open   = getValue(makeField(ticker, "open"), idx),
        .high   = getValue(makeField(ticker, "high"), idx),
        .low    = getValue(makeField(ticker, "low"), idx),
        .close  = getValue(makeField(ticker, "close"), idx),
        .volume = getValue(makeField(ticker, "volume"), idx)
    };
}

std::string ExecutionEngine::makeField(const std::string& ticker, const std::string& field)
{
    return ticker + "_" + field;
}

double ExecutionEngine::bpsToFrac(double bps) const {
    return bps / 10000.0;
}

SlippageParams ExecutionEngine::getSlippageParams(const config& cfg,
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

void ExecutionEngine::fillPosition(trade& trade){
    return;
}

void ExecutionEngine::initOrder(trade& trade){
    
    const Bar bar = getBar(trade.ticker,trade.timestamp.entryIdx);

    const double open = bar.open;
    const double bps  = bpsToFrac(cfg_.stopLossBps);

    const double slMult = (trade.type == ordertype::Buy) ? (1.0 - bps) : (1.0 + bps);
    trade.price.stopLossPrice = open * slMult;

    const double risk = cfg_.riskPerTrade * account_.availableFreeCash() * 0.01;
    const double dist = std::abs(open - trade.price.stopLossPrice);

    if (dist <= 0.0) {
        throw std::runtime_error("Stop-loss distance is zero");
    }

    trade.qty.targetQty = risk / dist;
    trade.isPending = (trade.qty.remainingQty() > 0.0);

}

double ExecutionEngine::getAdjPrice(trade& trade,double const & open,double const& impactBps){
    double price{0.0};

    if (trade.type == ordertype::Sell){
        price = open*(1.0 - bpsToFrac(cfg_.spread) - bpsToFrac(impactBps));
    } else price = open*(1.0 + bpsToFrac(cfg_.spread) + bpsToFrac(impactBps));

    return price;
}

void ExecutionEngine::stopLoss(trade& trade, double entryPrice) {
    const double bps = bpsToFrac(cfg_.stopLossBps);

    if (trade.type == ordertype::Buy) {
        trade.price.stopLossPrice = entryPrice * (1.0 - bps);
    } else {
        trade.price.stopLossPrice = entryPrice * (1.0 + bps);
    }
}

double ExecutionEngine::calcImpactBps(double qty, double volume) const {
    if (volume <= 0.0 || qty <= 0.0) {
        return 0.0;
    }

    const double participation = std::min(qty / volume, cfg_.slippage.maxParticipation);

    return cfg_.slippage.impactCoef * std::sqrt(participation);
}

void ExecutionEngine::applyCashEffect(trade& trade, double qty, double price){
    double cashCostMultiplier = (1+bpsToFrac(cfg_.commissionBps));
    double positionCost{0};
    if (trade.type == ordertype::Buy) {
        positionCost = qty * price * cashCostMultiplier;
    } else {
        positionCost = qty * price * cfg_.shortInitMargin * cashCostMultiplier;
    }

    account_.updateReservedMargin(positionCost/cashCostMultiplier);
    trade.usedMargin += positionCost/cashCostMultiplier;
    account_.updateFreeCash(-positionCost);
}

double ExecutionEngine::calcQtyCash(const trade& trade, double price) const {
    double cashCostMultiplier = (1+bpsToFrac(cfg_.commissionBps));
    if (price <= 0.0) {
        throw std::runtime_error("Price must be > 0");
    }

    if (trade.type == ordertype::Buy) {
        return account_.availableFreeCash() / (price * cashCostMultiplier);
    } 
    
    return account_.availableFreeCash() / (price * cfg_.shortInitMargin * cashCostMultiplier);
}

bool ExecutionEngine::FOK(trade& trade) {
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
    const double qtyCash = calcQtyCash(trade,price);

    const bool canFillByLiquidity = (needQty <= qtyLiq);
    const bool canFillByCash = (needQty <= qtyCash);

    if (!canFillByLiquidity || !canFillByCash) {
        return false;
    }

    trade.qty.filledQty = needQty;
    trade.price.avgPrice = price;

    trade.isPending = false;
    applyCashEffect(trade,needQty,price);

    stopLoss(trade, price);
    trade.executionPrice.push_back({price, needQty});

    return true;
}

/**
------------------------- Good till Cancelled ----------------------
 @brief This function serves to fill market order candle by candle.
        If the order is too big to execute without significant price change,
        fill will be done in the following candles  in order to get a good avgPrice
*/
void ExecutionEngine::executeGTCBar(trade& trade,size_t idx){

    const Bar bar = getBar(trade.ticker,idx);

    if (bar.volume <= 0) return;

    double qtyLiq = cfg_.slippage.maxParticipation*bar.volume;
    double qty = std::min(qtyLiq, trade.qty.remainingQty());
    if (qty <= 0.0) return;

    double price = getAdjPrice(trade, bar.open, calcImpactBps(qty, bar.volume));
    double qtyCash = calcQtyCash(trade, price);

    bool canFillByCash = (qtyCash >= qty);

    if (!canFillByCash){
        throw std::runtime_error("Insufficient freeCash balance to cover trade.");
    }

    qty = std::min(qty, qtyCash);

    if (qty <= 0.0) return;

    trade.qty.filledQty += qty;
    trade.executionPrice.push_back({price,qty});

    averageExecutionPrice(trade);
    stopLoss(trade, trade.price.avgPrice);
    applyCashEffect(trade,qty,price);

    if (trade.qty.remainingQty() <= 0){
        trade.isPending=false;
    }
}

void ExecutionEngine::averageExecutionPrice(trade& trade) const {
    const auto& fills = trade.executionPrice;

    double totalQty{0};
    double totalNotional{0};

    for (const auto& fill : fills){
        totalNotional += fill.price * fill.qty;
        totalQty += fill.qty;
    }

    if (totalQty <= 0) {
        throw std::runtime_error("division by zero, filled qty is null");
    }

    trade.price.avgPrice = totalNotional/totalQty;
}
