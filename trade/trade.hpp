#include <string>
#include <vector>
#include "marketTypes/marketTypes.hpp"
struct trade {

    double availableCash;

    std::string ticker;
    timestamp timestamp;

    std::vector<PriceQty> executionPrice;

    ordertype type;
    QtyState qty;
    PriceState price;

    bool isPending{false};


    trade(const std::string& ticker_);
};