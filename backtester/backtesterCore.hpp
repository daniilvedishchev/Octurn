#include <unordered_map>
#include "interpreter/Interpreter.hpp"
#include "config/cfgTemp.hpp"

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

struct trade {

    std::string ticker;
    ordertype type;
    timestamp timestamp;

    std::unordered_map<std::string,double> executionPrice;

    double targetQty = 0.0;
    double filledQty = 0.0;
    double remainingQty = 0.0;

    double avgPrice = 0.0;
    double stopLossPrice = 0.0;

    double participation = 0.0;
    double brokerCommisionCash = 0.0;

    trade(const std::string& ticker_);
};

class backtesterCore {
    private:

        std::unordered_map<std::string, AnyValue> data_;
        config cfg_;
        std::vector<trade> history_;
        std::vector<std::string> timestampVec_;
        size_t maxSize_;

    public:

        backtesterCore(std::unordered_map<std::string, AnyValue>& data,config& cfg);
        void setEntryExit(size_t& i, trade& trade, action actiontype);
        void populateTradeFromCfg(trade& trade);

        SlippageParams getSlippageParams(const config& cfg,
                                 const std::unordered_map<Slippage,std::unordered_map<std::string,double>>& SlippageCfg,
                                 double volume);

        double getValue(const std::string& key, size_t idx);
        void killOrFill(trade& trade, size_t idxBias);

        double bpsToFrac(double bps) const;

        bool stopLossHit();
        void fillPosition(trade& trade);
        std::string idx2stamp(size_t& idx);
        void execute(const std::string& ticker,const std::vector<bool>& entries,const std::vector<bool>& exits);
};
