#include "backtesterCore.hpp"
#include "execution/ExecutionEngine.hpp"

backtesterCore::backtesterCore(std::unordered_map<std::string, AnyValue>& data, config& cfg, MarketDataView& viewer,ExecutionEngine& executionLayer) : data_(std::move(data)), cfg_(std::move(cfg)), account_(account(cfg_.equity)), marketViewer_(viewer), executionLayer_(executionLayer) {
    timestampVec_ = *std::get_if<std::vector<std::string>>(&data_["timestamp"]);
    maxSize_ = timestampVec_.size();
    openTrades_.reserve(maxSize_);
};

std::string backtesterCore::idx2stamp(size_t& idx){
    return timestampVec_[idx];
};

void backtesterCore::setEntryExit(size_t& i, trade& trade, action actiontype){
    if (actiontype == action::Entry) {
        trade.timestamp.entryIdx = i+1;
        trade.timestamp.entryTimestamp = idx2stamp(trade.timestamp.entryIdx);
    } else {
        trade.timestamp.exitIdx = i+1;
        trade.timestamp.exitTimestamp = idx2stamp(trade.timestamp.exitIdx);
    }
}

void backtesterCore::checkEntryExit(size_t iteration,trade& trade_, bool inTrade, const std::vector<bool>& entries, const std::vector<bool>& exits){
    if (!inTrade){
            if (entries[iteration] == true && exits[iteration] == false){
                inTrade = true;
                setEntryExit(iteration,trade_,action::Entry);
            }
        } else {
            if (exits[iteration] == true){
                setEntryExit(iteration,trade_,action::Exit);
                openTrades_.push_back(trade_);
                inTrade = false;
                trade_ = trade(trade_.ticker);
            }
        }
}

void backtesterCore::markOpenTradesToMarket(double marketPrice){
    for (auto trade:openTrades_){
        account_.markToMarket(marketPrice,trade.price.avgPrice,trade.qty.filledQty,trade.type);
    }
}

void backtesterCore::execute(const std::string& ticker,const std::vector<bool>& entries,const std::vector<bool>& exits){

    bool inTrade{false};
    trade trade(ticker);

    size_t vectSize{entries.size()};

    if (vectSize < 2) {
        throw std::runtime_error("Insufficient data to evaluate strategy");
        return;
    }

    for (size_t i{0}; i < vectSize - 1; i++){
        double marketPrice = executionLayer_.getValue(marketViewer_.makeField(ticker, "open"), i);
        checkEntryExit(i,trade,inTrade,entries,exits);
        markOpenTradesToMarket(marketPrice);
        
    }
}
