#include "backtesterCore.hpp"

backtesterCore::backtesterCore(std::unordered_map<std::string, AnyValue>& data,config& cfg): data_(std::move(data)) ,cfg_(std::move(cfg)) {};

trade::trade(std::string& ticker) : ticker(std::move(ticker)){};

std::string backtesterCore::idx2stamp(int& idx){
    return (*std::get_if<std::vector<std::string>>(&data_["timestamp"]))[static_cast<int>(idx)];
};

void backtesterCore::execute(std::string& ticker,std::vector<AnyValue>& entries,std::vector<AnyValue>& exits){

    bool enteredTrade{false};
    trade newtrade(ticker);

    size_t vectSize{entries.size()};
    for (size_t i{0}; i < vectSize; i++){
        if (!enteredTrade){
            if (*std::get_if<bool>(&entries[i]) == true && *std::get_if<bool>(&exits[i]) == false){
                enteredTrade = true;
                newtrade.timestamp.entryIdx = static_cast<int>(i);
                newtrade.timestamp.entryTimestamp = idx2stamp(newtrade.timestamp.entryIdx);
            }
        } else {
            if (*std::get_if<bool>(&exits[i]) == true){
                newtrade.timestamp.exitIdx = static_cast<int>(i);
                newtrade.timestamp.exitTimestamp = idx2stamp(newtrade.timestamp.exitIdx);
                history_.push_back(newtrade);
                enteredTrade = false;
                newtrade = trade(ticker);
            }
        }
    }
}