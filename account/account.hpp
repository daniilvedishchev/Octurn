struct profitAndLoss {
    double realizedPnL;
    double unrealizedPnL;
};

class account {
private:

    profitAndLoss profitAndLoss;

    double equity;
    double cash;
    double reservedMargin;
public:
    account(double equity);

    void updateEquity();
};