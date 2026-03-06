#include <unordered_map>
#include "interpreter/Interpreter.hpp"

struct timestamp {
    std::string entryTimestamp;
    size_t entryIdx;
    std::string exitTimestamp;
    size_t exitIdx;
};

struct trade {

    std::string ticker;
    timestamp timestamp;

    double avgExecutionPrice;
    double stopLoss;
    double positionSize;

    trade(std::string& ticker);
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
        std::string idx2stamp(size_t& idx);
        void execute(std::string& ticker,std::vector<bool>& entries,std::vector<bool>& exits);
};
