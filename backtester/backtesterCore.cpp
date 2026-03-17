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


void backtesterCore::fillPosition(trade& trade){

    size_t bias{0};
    size_t idx = trade.timestamp.entryIdx;

    cfg_.slippage = getSlippageParams(cfg_,slippageTable);


}

void backtesterCore::initGTC(trade& trade){

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

void backtesterCore::executeGTCBar(trade& trade,size_t idx){

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