#include "backtesterCore.hpp"

backtesterCore::backtesterCore(std::unordered_map<std::string, AnyValue>& data,config& cfg): data_(std::move(data)), cfg_(std::move(cfg)) {
    timestampVec_ = *std::get_if<std::vector<std::string>>(&data_["timestamp"]);
    maxSize_ = timestampVec_.size();
    history_.reserve(maxSize_);
};

trade::trade(const std::string& ticker_){ticker = ticker_;};

std::string backtesterCore::idx2stamp(size_t& idx){
    return timestampVec_[idx];
};

double backtesterCore::getValue(const std::string& key, size_t idx){
    auto it = data_.find(key);
    if (it == data_.end())
        throw std::runtime_error(std::format("Series {} not found", key));

    const auto& series = std::get<std::vector<double>>(it->second);

    if (idx >= series.size())
        throw std::runtime_error("Index out of bounds");

    return series[idx];
}

std::string makeField(const std::string& ticker, const std::string& field)
{
    return ticker + "_" + field;
}

double backtesterCore::bpsToFrac(double bps) const {
    return bps / 10000.0;
}


SlippageParams backtesterCore::getSlippageParams(const config& cfg,
                                 const std::unordered_map<Slippage,std::unordered_map<std::string,double>>& SlippageCfg,
                                 double volume)
{
    auto it = SlippageCfg.find(cfg.slippageRegime);
    if (it == SlippageCfg.end()) {
        throw std::runtime_error("Unknown slippageRegime");
    }
    if (volume <= 0.0) {
        throw std::runtime_error("Volume must be > 0");
    }

    return {
        it->second.at("max_participation"),
        it->second.at("impact_coef")
    };
}


void backtesterCore::fillPosition(trade& trade){
    size_t bias{0};
    size_t idx = trade.timestamp.entryIdx;
    SlippageParams globalSlippageEnv = getSlippageParams(cfg_,
                                        SlippageCfg,
                                        getValue(makeField(trade.ticker,"volume"),idx));
    killOrFill(trade,idx+bias);

}

void backtesterCore::killOrFill(trade& trade, size_t idxBias){

    if (trade.targetQty == trade.filledQty){
        return;
    }

    if (idxBias == 0){
        SlippageParams slipParams = getSlippageParams(cfg_,SlippageCfg,);
    }

    const double maxParticipation = slipParams.maxParticipation;
    const double impactB          = slipParams.impactCoef;
    const double accountRisk      = cfg_.equity * cfg_.riskPerTrade;

    double estExecutionPrice = 0.0;
    double estStopLossPrice  = 0.0;

    if (trade.type == ordertype::Buy) {
        estExecutionPrice = open * (1.0 + bpsToFrac(cfg_.spreadBps));
        estStopLossPrice  = estExecutionPrice * (1.0 - bpsToFrac(cfg_.stopLossBps));
    } else {
        estExecutionPrice = open * (1.0 - bpsToFrac(cfg_.spreadBps));
        estStopLossPrice  = estExecutionPrice * (1.0 + bpsToFrac(cfg_.stopLossBps));
    }

    const double estRiskPerShare = std::abs(estExecutionPrice - estStopLossPrice);
    if (estRiskPerShare <= 0.0) {
        throw std::runtime_error("Estimated risk per share must be > 0");
    }

    trade.targetQty = accountRisk / estRiskPerShare;

    const double estParticipation = trade.targetQty / volume;
    const double cappedParticipation = std::min(estParticipation, maxParticipation);

    // 4) Impact po capped participation
    const double impactBps = std::sqrt(cappedParticipation) * impactB;

    // 5) Final execution price + stop
    if (trade.type == ordertype::Buy) {
        trade.avgPrice = open * (1.0 + bpsToFrac(cfg_.spreadBps + impactBps));
        trade.stopLossPrice  = trade.avgPrice * (1.0 - bpsToFrac(cfg_.stopLossBps));
    } else {
        trade.avgPrice = open * (1.0 - bpsToFrac(cfg_.spreadBps + impactBps));
        trade.stopLossPrice  = trade.avgPrice * (1.0 + bpsToFrac(cfg_.stopLossBps));
    }

    const double riskPerShare = std::abs(trade.avgPrice - trade.stopLossPrice);
    if (riskPerShare <= 0.0) {
        throw std::runtime_error("Risk per share must be > 0");
    }

    const double qtyRisk = accountRisk / riskPerShare;

    const double qtyCash =
        cfg_.equity / (trade.avgPrice * (1.0 + bpsToFrac(cfg_.commissionBps)));

    const double qtyLiquidity = volume * maxParticipation;

    trade.filledQty += std::floor(std::min({qtyRisk, qtyCash, qtyLiquidity}));

    double bar{0};
    trade.brokerCommisionCash += trade.filledQty*volume;

    while (trade.targetQty != trade.filledQty){
        bar++;
        size_t idx = trade.timestamp.entryIdx + bar;

        double open = getValue(makeField(trade.ticker, "open"), idx);
        double volume = getValue(makeField(trade.ticker, "volume"), idx);




    }
}

void backtesterCore::populateTradeFromCfg(trade& trade){

    double open = getValue(makeField(trade.ticker, "open"), trade.timestamp.entryIdx);
    double volume = getValue("Volume", trade.timestamp.entryIdx);
    
    double accountRisk = cfg_.equity * cfg_.riskPerTrade;
    double riskPerShare = open * cfg_.stopLossBps / 10000.0;

    trade.qty = accountRisk / riskPerShare;

    trade.participation = trade.qty/volume;

}

void backtesterCore::setEntryExit(size_t& i, trade& trade, action actiontype){
    if (actiontype == action::Entry){
        trade.timestamp.entryIdx = i+1;
        trade.timestamp.entryTimestamp = idx2stamp(trade.timestamp.entryIdx);
    } else {
        trade.timestamp.exitIdx = i+1;
        trade.timestamp.exitTimestamp = idx2stamp(trade.timestamp.exitIdx);
    }
}

void backtesterCore::execute(const std::string& ticker,const std::vector<bool>& entries,const std::vector<bool>& exits){

    bool enteredTrade{false};
    trade newtrade(ticker);

    size_t vectSize{entries.size()};

    if (vectSize < 2) {
        throw std::runtime_error("Insufficient data to evaluate strategy");
        return;
    }

    for (size_t i{0}; i < vectSize - 1; i++){
        if (!enteredTrade){
            if (entries[i] == true && exits[i] == false){
                enteredTrade = true;
                setEntryExit(i,newtrade,action::Entry);
                populateTradeFromCfg(newtrade);
            }
        } else {
            if (exits[i] == true){
                setEntryExit(i,newtrade,action::Exit);
                history_.push_back(newtrade);
                enteredTrade = false;
                newtrade = trade(ticker);
            }
        }
    }
}