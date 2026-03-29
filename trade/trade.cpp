#include "trade.hpp"

trade::trade(const std::string& ticker_):ticker(ticker_){
    generateTradeID();
}

void trade::generateTradeID(){
    ID = "[TRADE] " + ticker + " - " + timestamp.entryTimestamp; 
}

void trade::changeTradeStatusToClosed(){
    status = tradeStatus::CLOSED;
}