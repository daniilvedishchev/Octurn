#include "account/account.hpp"

account::account(double equity) : equity(equity) {}

void account::updateEquity(){
    equity = profitAndLoss.unrealizedPnL + cash;
}