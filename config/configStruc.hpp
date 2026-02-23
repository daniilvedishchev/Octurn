/**
 * @brief This file contains trade configuration all parameters below are available for usage
 * @details
 *      - positionSize depends on chosen mode either % of equity or fixed sum
 *      - comissionBps - buy/sell broker commission
 *      - slippageBps - buy/sell slippage from orderbook
 *      - positionMode - fixed or equity percentage 
*/

#pragma once

enum class Mode {
    fixed,
    percent
};

struct config{
    double equity;
    double commissionBps;
    double slippageBps;
    double positionSize;

    Mode positionMode;
};
