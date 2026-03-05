#include "backtesterCore.hpp"

backtesterCore::backtesterCore(std::unordered_map<std::string, AnyValue>& data,config& cfg): data_(std::move(data)) ,cfg_(std::move(cfg)) {};

trade::trade(std::string& ticker) : 
    ticker(std::move(ticker)){};

void backtesterCore::execute(std::string& ticker,std::vector<AnyValue>& entries,std::vector<AnyValue>& exits){

    bool enteredTrade{false};
    trade trade(ticker);

    size_t vectSize{entries.size()};
    for (size_t i{0}; i < vectSize; i++){
        if (!enteredTrade){
            if (*std::get_if<bool>(&entries[i]) == true && *std::get_if<bool>(&exits[i]) == false){
                enteredTrade = true;
                trade.timestamp.enstamp = (*std::get_if<std::vector<std::string>>(&data_["timestamp"]))[static_cast<int>(i)];
            }
        } else {
            if (*std::get_if<bool>(&exits[i]) == true){
                enteredTrade = false;
                trade.timestamp.exstamp = (*std::get_if<std::vector<std::string>>(&data_["timestamp"]))[static_cast<int>(i)];
                history_.push_back(trade);
            }
        }
        
    }
}