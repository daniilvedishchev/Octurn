#include "account/account.hpp"

account::account(double equity) : equity(equity), freeCash(equity), reservedMargin(0.0) {
    profitAndLoss.realizedPnL = 0.0;
    profitAndLoss.realizedPnL = 0.0;
}

void account::updateEquity(){
    equity = profitAndLoss.unrealizedPnL + freeCash + reservedMargin;
}

double account::markToMarket(double currentPrice, double entryPrice, double qty, ordertype side){
    if (side == ordertype::Buy) {
        return (currentPrice - entryPrice) * qty;
    } else {return (entryPrice - currentPrice) * qty;}
}

void account::realizeTradePnL(double tradePnL){
    profitAndLoss.realizedPnL += tradePnL;
    profitAndLoss.unrealizedPnL -= tradePnL;
    freeCash += tradePnL;
}

double account::availableFreeCash(){
    return freeCash;
}

void account::updateReservedMargin(double amount){
    reservedMargin += amount;
}

void account::updateFreeCash(double amount){
    freeCash += amount;
}