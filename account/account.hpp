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

    void updateEquity();
    double markToMarket(double currentPrice, double entryPrice, double qty, ordertype side);
    void realizeTradePnL(double unrealizedPnL);
    double availableFreeCash();
    void account::updateFreeCash(double amount);
    void account::updateReservedMargin(double amount);
};