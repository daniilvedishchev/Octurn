#include <unordered_map>
#include "interpreter/Interpreter.hpp"

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

    double executionPrice;
    double qty;
    double stopLossPrice;
    double positionSize;

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
        bool stopLossHit();
        std::string idx2stamp(size_t& idx);
        void execute(const std::string& ticker,const std::vector<bool>& entries,const std::vector<bool>& exits);
};
