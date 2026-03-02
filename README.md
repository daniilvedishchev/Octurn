# Octurn

**Octurn** is a C++20 strategy engine and domain-specific language (DSL) for quantitative trading research.

It is designed to make strategies:

- **easy to express**
- **easy to inspect**
- **easy to test**
- and eventually **easy to connect to execution systems**

Octurn sits between raw market data and production trading infrastructure.  
Its purpose is to reduce the gap between a trading idea and a strategy that can be evaluated, debugged, and later deployed.

---

## Why Octurn

Most strategy workflows are still fragmented:

- research logic is spread across scripts and notebooks,
- indicators are hardcoded into application logic,
- strategy rules are not portable,
- and execution often depends on a completely separate stack.

Octurn aims to solve this by providing:

- a **compact DSL** for writing strategies,
- a **native C++ engine** for evaluation and backtesting,
- a **modular architecture** for parsing, interpreting, and extending strategy logic,
- and a base for future **execution-layer integrations**.

---

## Core Idea

Instead of writing strategy logic directly in large application codebases, Octurn lets you describe:

- configuration,
- data sources,
- parameters,
- indicators,
- entry rules,
- exit rules

in a dedicated strategy language.

That strategy is then interpreted by the engine and transformed into a structured runtime output.

---

## Example Strategy

```octurn
config {
  equity: 100
  positionSize: 1
  slippageBps: 10
}

data [
  { ticker: AAPL timespan: day multiplier: 1 from: 2025-09-01 to: 2025-10-27 },
  { ticker: MSFT timespan: day multiplier: 1 from: 2025-08-01 to: 2025-10-27 }
]

strategy SimpleMA {
  parameters {
    fast_ma: 5
    slow_ma: 30
  }

  indicators {
    RSI1 = RSI(AAPL_close, 12)
  }

  entry {
    when RSI1 > 50
  }

  exit {
    when RSI1 > 250
  }
}

Octurn Runtime
==============

Config
------
equity        : 100
positionSize  : 1
slippageBps   : 10

Data Sources
------------
- AAPL | timespan: day | multiplier: 1 | from: 2025-09-01 | to: 2025-10-27 | bars: 40
- MSFT | timespan: day | multiplier: 1 | from: 2025-08-01 | to: 2025-10-27 | bars: 61

Strategy
--------
name          : SimpleMA
parameters    : fast_ma = 5, slow_ma = 30

Indicators
----------
RSI1          : length = 40
RSI1 head     : [nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan, nan]
RSI1 tail     : [49.031561, 51.622211, 48.391995, 56.071202, 67.062637, 67.534664, 59.901390, 61.154273, 64.612738, 69.999618]

Signals
-------
Entry true at indices : [12..27, 31, 33..39]
Exit true at indices  : []

Entry count           : 23
Exit count            : 0

Materialized Series
-------------------
AAPL_open, AAPL_high, AAPL_low, AAPL_close, AAPL_volume, AAPL_timestamp
MSFT_open, MSFT_high, MSFT_low, MSFT_close, MSFT_volume, MSFT_timestamp

Flags
-----
Entry  : [false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, true, false, true, true, true, true, true, true, true]
Exit   : [false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false]
