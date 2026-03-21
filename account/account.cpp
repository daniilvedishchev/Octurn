#include "account/account.hpp"

account::account(double equity) : equity(equity) {}

void account::updateEquity(){
    equity = profitAndLoss.unrealizedPnL + cash;
}

void account::markToMarket(double currentPrice, double entryPrice, double qty, ordertype side){
    if (side == ordertype::Buy) {
        profitAndLoss.unrealizedPnL = (currentPrice - entryPrice) * qty;
    } else {profitAndLoss.unrealizedPnL = (entryPrice - currentPrice) * qty;}

    updateEquity();
}