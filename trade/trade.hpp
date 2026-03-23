#include <string>
#include <vector>
#include "marketTypes/marketTypes.hpp"
struct trade {

    std::string ticker;
    timestamp timestamp;

    std::vector<PriceQty> executionPrice;

    ordertype type;
    QtyState qty;
    PriceState price;

    bool isPending;

    double usedMargin;
    double borrowAccrued;

    trade(const std::string& ticker_);
};