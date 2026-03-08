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

void backtesterCore::populateTradeFromCfg(trade& trade){

    auto key = trade.ticker + "close";
    auto it = data_.find(key);
    auto itVol = data_.find("Volume");

    if (it == data_.end()){
        throw std::runtime_error(std::format("Unable to get price at idx:{}, close field doesn't exist",trade.timestamp.entryIdx));
    }

    double close = std::get<std::vector<double>>(it->second)[trade.timestamp.entryIdx];
    double volume = std::get<std::vector<double>>(itVol->second)[trade.timestamp.entryIdx];

    trade.qty = cfg_.positionSize/close;

    trade.stopLossPrice = close *((trade.type == ordertype::Sell) ? 1+(cfg_.riskPerTrade)/100 : 1-(cfg_.riskPerTrade)/100);

    double participation = trade.qty/volume;
    


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