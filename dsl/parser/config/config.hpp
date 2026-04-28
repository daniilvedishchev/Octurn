#include "dsl/token/TokenTypes.hpp"
#include "dsl/fields/session/session.hpp"
#include "dsl/fields/mode/mode.hpp"
#include "dsl/fields/execution/execution.hpp"
#include "dsl/datetime/datetime.hpp"
#include "dsl/datetime/range.hpp"
#include "dsl/fields/fill/fill.hpp"
#include "namespace/namespace.hpp"
#include "dataLayer/broker/brokers.hpp"

struct config {
    mode mode;
    session session;
    execution execution;
    brokers broker;
    range range;
    fill fill;
    
    Octurn::percent slippage;
    Octurn::percent brokerfee;
    Octurn::percent maxAccountLoss;
    Octurn::percent dailyMaxLoss;
    Octurn::percent risk;

    double capital;
};