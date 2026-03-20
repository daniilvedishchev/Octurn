#include <unordered_map>
#include <algorithm>
#include "interpreter/Interpreter.hpp"
#include "config/config.hpp"
#include "trade/trade.hpp"

class backtesterCore {
    private:

        std::unordered_map<std::string, AnyValue> data_;
        std::vector<trade> history_;
        std::vector<std::string> timestampVec_;
        size_t maxSize_;

        void setEntryExit(size_t& i, trade& trade, action actiontype);

        std::string idx2stamp(size_t& idx);

    public:
        config cfg_;
        backtesterCore(std::unordered_map<std::string, AnyValue>& data,config& cfg);
        void execute(const std::string& ticker,const std::vector<bool>& entries,const std::vector<bool>& exits);
};
