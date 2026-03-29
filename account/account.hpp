#pragma once
#include "marketTypes/marketTypes.hpp"

struct profitAndLoss {
    double realizedPnL;
    double unrealizedPnL;
};

class account {
private:

    profitAndLoss profitAndLoss;

    double equity;
    double freeCash;
    double reservedMargin;
public:
    account(double equity);
    double availableFreeCash();

    double markToMarket(double currentPrice, double entryPrice, double qty, ordertype side);
    
    void updateEquity();
    void realizeTradePnL(double unrealizedPnL);
    void updateFreeCash(double amount);
    void updateReservedMargin(double amount);
};