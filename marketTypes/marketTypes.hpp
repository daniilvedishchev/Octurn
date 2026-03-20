#include <string>

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

struct Bar {
    double open = 0.0;
    double high = 0.0;
    double low = 0.0;
    double close = 0.0;
    double volume = 0.0;
};
