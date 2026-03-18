#include "executionEngine.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <stdexcept>

#include "config/slippageTable.hpp"

executionEngine::executionEngine(std::unordered_map<std::string, AnyValue>& data, config& cfg)
    : data_(data), cfg_(cfg) {}

double executionEngine::getValue(const std::string& key, size_t idx){
    auto it = data_.find(key);
    if (it == data_.end())
        throw std::runtime_error(std::format("Series {} not found", key));

    const auto& series = std::get<std::vector<double>>(it->second);

    if (idx >= series.size())
        throw std::runtime_error("Index out of bounds");

    return series[idx];
}

std::string executionEngine::makeField(const std::string& ticker, const std::string& field)
{
    return ticker + "_" + field;
}

double executionEngine::bpsToFrac(double bps) const {
    return bps / 10000.0;
}

SlippageParams executionEngine::getSlippageParams(const config& cfg,
    const std::unordered_map<Slippage,std::unordered_map<std::string,double>>& slippageTable)
{
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

    size_t bias{0};
    size_t idx = trade.timestamp.entryIdx;

    cfg_.slippage = getSlippageParams(cfg_,slippageTable);


}

void executionEngine::baseTradeInit(trade& trade){

    const double open = getValue(makeField(trade.ticker,"open"), trade.timestamp.entryIdx);
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
        price = open*(1.0 + bpsToFrac(cfg_.spread) + bpsToFrac(impactBps));
    } else price = open*(1.0 - bpsToFrac(cfg_.spread) - bpsToFrac(impactBps));

    return price;
}

void executionEngine::FOK(trade& trade){

    const double open = getValue(makeField(trade.ticker,"open"), trade.timestamp.entryIdx);
    const double volume = getValue(makeField(trade.ticker,"volume"),trade.timestamp.entryIdx);

    const double bps  = bpsToFrac(cfg_.stopLossBps);

    double qtyLiq = cfg_.slippage.maxParticipation*getValue(makeField(trade.ticker,"volume"),trade.timestamp.entryIdx);
    double qtyCash = cfg_.equity/open;

    double qty = std::min(qtyLiq,qtyCash);

    if (trade.qty.targetQty < qty){
        double impactBps = cfg_.slippage.impactCoef*sqrt(std::min(qty/volume,cfg_.slippage.maxParticipation));
        double const& price =  getAdjPrice(trade,open,impactBps);
    } else throw std::runtime_error("Unable to fullfill an order");

}

/**
------------------------- Good till Cancelled ----------------------
 @brief This function serves to fill market order candle by candle.
        If the order is too big to execute without significant price change,
        fill will be done in the following candles  in order to get a good avgPrice
*/
void executionEngine::executeGTCBar(trade& trade,size_t idx){

    const double open = getValue(makeField(trade.ticker,"open"),idx);
    const double volume = getValue(makeField(trade.ticker,"volume"),idx);

    if (volume <= 0) return;

    double qtyLiq = cfg_.slippage.maxParticipation*getValue(makeField(trade.ticker,"volume"),idx);
    double qty = std::min(qtyLiq,trade.qty.remainingQty());

    double impactBps = cfg_.slippage.impactCoef*sqrt(std::min(qty/volume,cfg_.slippage.maxParticipation));
    double price{0.0};

    if (trade.type == ordertype::Sell){
        price = open*(1.0 + bpsToFrac(cfg_.spread) + bpsToFrac(impactBps));
    } else price = open*(1.0 - bpsToFrac(cfg_.spread) - bpsToFrac(impactBps));

    double qtyPrice = cfg_.equity/price;

    qty = std::min(qty,qtyPrice);

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
