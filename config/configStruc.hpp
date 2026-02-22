/**
 * @brief This file contains trade configuration all parameters below are available for usage
 * @details
 *      - positionSize depends on chosen mode either % of equity or fixed sum
 *      - comissionBps - buy/sell broker commission
 *      - slippageBps - buy/sell slippage from orderbook
 *      - positionMode - fixed or equity percentage 
*/


enum class Mode {
    fixed,
    percent
};

enum class cfgKey{
    equity,comissionBps,slippageBps,positionSize,positionMode
};

struct config{
    double equity;
    double commissionBps;
    double slippageBps;
    double positionSize;

    Mode positionMode;
};
