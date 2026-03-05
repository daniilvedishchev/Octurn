#include <unordered_map>
#include "interpreter/Interpreter.hpp"


struct trade {
    std::string ticker;
    timestamp timestamp;
    double avgExecutionPrice;
    double stopLoss;
    double positionSize;

    trade(std::string& ticker);
};

struct timestamp {
    std::string enstamp;
    std::string exstamp;
};

class backtesterCore {
    private:
        std::unordered_map<std::string, AnyValue> data_;
        config cfg_;
        std::vector<trade> history_;
    public:
        backtesterCore(std::unordered_map<std::string, AnyValue>& data,config& cfg);
        void execute(std::string& ticker,std::vector<AnyValue>& entries,std::vector<AnyValue>& exits);
};
