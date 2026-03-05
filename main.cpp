#include <iostream>
#include <string>
#include "engine/octurn.hpp"

int main() {

    std::string script = "config { equity: 100 positionSize: 1 slippageBps: 10 } data [ { ticker: AAPL timespan: day multiplier: 1 from: 2025-09-01 to: 2025-10-27}, { ticker: MSFT timespan: day multiplier: 1 from: 2025-08-01 to: 2025-10-27} ] strategy SimpleMA { parameters { fast_ma: 5 slow_ma: 30 } indicators {RSI1=RSI(AAPL_close,12)} entry {when RSI1>50} exit {when RSI1>250}}";

    std::string api_key = "fOCoE61aBE9Ndpln9eFMWwAKPH682n4T";
    octurn engine = octurn(script, api_key);
    
    engine.run();
}
