#include "backtesterCore.hpp"

backtesterCore::backtesterCore(std::unordered_map<std::string, AnyValue>& data,config& cfg): data_(std::move(data)), cfg_(std::move(cfg)) {
    timestampVec_ = *std::get_if<std::vector<std::string>>(&data_["timestamp"]);
    maxSize_ = timestampVec_.size();
    history_.reserve(maxSize_);
};

trade::trade(std::string& ticker) : ticker(std::move(ticker)){};

std::string backtesterCore::idx2stamp(size_t& idx){
    return timestampVec_[idx];
};

void backtesterCore::populateTradeFromCfg(trade& trade){
    trade.stopLossPrice = std::get<std::vector<double>>(data_[trade.ticker+"close"]) \
    [trade.timestamp.entryIdx]*((trade.type == ordertype::Sell) ? 1+(cfg_.riskPerTrade)/100 : 1-(cfg_.riskPerTrade)/100);
}

void backtesterCore::setEntryExit(size_t& i, trade& trade, action&& action){
    if (action == action::Entry){
        trade.timestamp.entryIdx = i+1;
        trade.timestamp.entryTimestamp = idx2stamp(trade.timestamp.entryIdx);
    } else {
        trade.timestamp.exitIdx = i+1;
        trade.timestamp.exitTimestamp = idx2stamp(trade.timestamp.exitIdx);
    }
}

void backtesterCore::execute(std::string& ticker,std::vector<bool>& entries,std::vector<bool>& exits){

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