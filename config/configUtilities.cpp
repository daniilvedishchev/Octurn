#include "config/configUtilities.hpp"
#include "config/config.hpp"
void stringToSlippage(const std::string& slippage, config& cfg) {
    if (slippage == "base") {
        cfg.slippageRegime = Slippage::base;
    } else if (slippage == "optimistic") {
        cfg.slippageRegime = Slippage::optimistic;
    } else if (slippage == "pessimistic") {
        cfg.slippageRegime = Slippage::pessimistic;
    }
}
