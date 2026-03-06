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

void backtesterCore::execute(std::string& ticker,std::vector<bool>& entries,std::vector<bool>& exits){

    bool enteredTrade{false};
    trade newtrade(ticker);

    size_t vectSize{entries.size()};
    for (size_t i{0}; i < vectSize; i++){
        if (!enteredTrade){
            if (entries[i] == true && exits[i] == false){
                enteredTrade = true;
                newtrade.timestamp.entryIdx = i;
                newtrade.timestamp.entryTimestamp = idx2stamp(newtrade.timestamp.entryIdx);
            }
        } else {
            if (exits[i] == true){
                newtrade.timestamp.exitIdx = i;
                newtrade.timestamp.exitTimestamp = idx2stamp(newtrade.timestamp.exitIdx);
                history_.push_back(newtrade);
                enteredTrade = false;
                newtrade = trade(ticker);
            }
        }
    }
}