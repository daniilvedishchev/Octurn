#include <string>
#include <vector>
#include "marketTypes/marketTypes.hpp"

struct trade {
    std::string ticker;
    std::string ID;

    double usedMargin;
    double borrowAccrued;

    timestamp timestamp;

    std::vector<PriceQty> executionPrice;

    ordertype type;
    QtyState qty;
    PriceState price;

    tradeStatus status;
    
    trade(const std::string& ticker_);
    
    void generateTradeID();
    void changeTradeStatusToClosed();
};