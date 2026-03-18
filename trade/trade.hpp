#include <string>
#include <vector>

struct timestamp {
    std::string entryTimestamp;
    size_t entryIdx;
    std::string exitTimestamp;
    size_t exitIdx;
};

enum class ordertype {
    Buy,Sell
};

enum class action {
    Entry,Exit
};

struct QtyState {

    double targetQty = 0.0;
    double filledQty = 0.0;

    double remainingQty() const {
        return std::max(0.0, targetQty - filledQty);
    }
};

struct PriceState {
    double avgPrice;
    double stopLossPrice;
};

struct PriceQty{
    double price;
    double qty;
};

struct trade {

    std::string ticker;
    timestamp timestamp;

    std::vector<PriceQty> executionPrice;

    ordertype type;
    QtyState qty;
    PriceState price;

    bool isPending{false};

    trade(const std::string& ticker_);
};