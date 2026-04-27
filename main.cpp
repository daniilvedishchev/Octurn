#include <iostream>
#include <string>
#include <cstdlib>
#include "engine/octurn.hpp"

int main() {

    const char* env_key = std::getenv("POLYGON_API_KEY");
    if (!env_key) {
        std::cerr << "Error: POLYGON_API_KEY environment variable is not set.\n";
        return 1;
    }
    std::string api_key = env_key;

    std::string script = "config { equity: 100 positionSize: 1 slippageBps: 10 } data [ { ticker: AAPL timespan: day multiplier: 1 from: 2025-09-01 to: 2025-10-27}, { ticker: MSFT timespan: day multiplier: 1 from: 2025-08-01 to: 2025-10-27} ] strategy SimpleMA { parameters { fast_ma: 5 slow_ma: 30 } indicators {RSI1=RSI(AAPL_close,12)} entry {when RSI1>50} exit {when RSI1>250}}";

    octurn engine = octurn(script, api_key);
    engine.run();
}
