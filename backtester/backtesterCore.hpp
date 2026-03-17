#include <unordered_map>
#include <algorithm>
#include "interpreter/Interpreter.hpp"
#include "config/config.hpp"

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
    double remainingQty = std::max(0.0,(targetQty-filledQty));
};

struct PriceState {
    double avgPrice;
    double stopLossPrice;
};

struct trade {

    std::string ticker;
    timestamp timestamp;

    std::unordered_map<std::string,double> executionPrice;

    ordertype type;
    QtyState qty;
    PriceState price;

    bool isPending{false};

    trade(const std::string& ticker_);
};

class backtesterCore {
    private:

        std::unordered_map<std::string, AnyValue> data_;
        std::vector<trade> history_;
        std::vector<std::string> timestampVec_;
        size_t maxSize_;

        double getValue(const std::string& key, size_t idx);

        SlippageParams getSlippageParams(const config& cfg,const std::unordered_map<Slippage,std::unordered_map<std::string,double>>& slippageTable);
        
        void setEntryExit(size_t& i, trade& trade, action actiontype);

        double bpsToFrac(double bps) const;

        std::string idx2stamp(size_t& idx);

    public:
        config cfg_;
        backtesterCore(std::unordered_map<std::string, AnyValue>& data,config& cfg);


        void GTC(trade& trade, size_t idxBias);

        bool stopLossHit();
        void fillPosition(trade& trade);
        void execute(const std::string& ticker,const std::vector<bool>& entries,const std::vector<bool>& exits);
};
