# Octurn

**Octurn** is a C++20 strategy engine and domain-specific language (DSL) for quantitative trading research.

It is built to make strategies:

- **easy to write**
- **easy to inspect**
- **easy to test**
- and eventually **easy to connect to execution systems**

Octurn sits between raw market data and production trading infrastructure.  
Its goal is to reduce the gap between a trading idea and a strategy that can be evaluated, debugged, and later deployed.

---

## Overview

Most quantitative workflows are still fragmented:

- strategy logic is spread across scripts and notebooks,
- indicators are hardcoded into application logic,
- backtesting is difficult to standardize,
- and execution often lives in a separate stack.

Octurn addresses this by providing:

- a **compact DSL** for expressing strategies,
- a **native C++ engine** for evaluation and backtesting,
- a **modular architecture** for parsing, interpreting, and extending strategy logic,
- and a clear path toward **execution-layer integration**.

---

## Why Octurn

Octurn is designed around a simple idea:

> a strategy should be a first-class object, not just a collection of scattered scripts.

Instead of embedding all logic directly into low-level code, Octurn lets you describe:

- configuration
- data sources
- parameters
- indicators
- entry conditions
- exit conditions

in a dedicated strategy language.

That strategy is then interpreted by the engine and transformed into structured runtime output.

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
```

---

## Example Runtime Output

A typical runtime output should be compact, readable, and useful for debugging.

```text
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
```

---

## Suggested JSON Output

As the engine evolves, the same runtime can also be exported in a machine-readable format for dashboards, APIs, or execution bridges.

```json
{
  "config": {
    "equity": 100,
    "positionSize": 1,
    "slippageBps": 10
  },
  "data": [
    {
      "ticker": "AAPL",
      "timespan": "day",
      "multiplier": 1,
      "from": "2025-09-01",
      "to": "2025-10-27",
      "bars": 40
    },
    {
      "ticker": "MSFT",
      "timespan": "day",
      "multiplier": 1,
      "from": "2025-08-01",
      "to": "2025-10-27",
      "bars": 61
    }
  ],
  "strategy": {
    "name": "SimpleMA",
    "parameters": {
      "fast_ma": 5,
      "slow_ma": 30
    }
  },
  "indicators": {
    "RSI1": {
      "length": 40,
      "head": ["nan", "nan", "nan", "nan", "nan"],
      "tail": [67.534664, 59.901390, 61.154273, 64.612738, 69.999618]
    }
  },
  "signals": {
    "entryCount": 23,
    "exitCount": 0,
    "entryTrueRanges": ["12-27", "31", "33-39"],
    "exitTrueRanges": []
  }
}
```

---

## Features

- **C++20 core**
- **strategy DSL**
- **indicator support**
- **runtime interpretation**
- **modular parser pipeline**
- **market data mappers**
- **backtesting-oriented architecture**
- future-ready for **execution integration**

---

## Architecture

Octurn is structured around a modular pipeline:

1. **Lexer**  
   Tokenizes strategy scripts.

2. **Parser**  
   Builds a structured representation of the strategy.

3. **AST / Nodes**  
   Represents expressions, parameters, indicators, and rules.

4. **Interpreter / Runtime**  
   Evaluates the strategy over market data.

5. **Technical Analysis Layer**  
   Computes indicators such as moving averages and RSI.

6. **Mappers / Data Layer**  
   Connects external market data into the runtime.

This design makes Octurn easier to extend, inspect, and integrate into larger systems.

---

## Product Vision

Octurn is not just a parser.

It is the foundation for a full quantitative research workflow:

- strategy definition in a clean DSL
- historical data ingestion
- indicator computation
- signal generation
- backtesting
- portfolio logic
- and eventually execution-layer connectivity

The long-term goal is to make Octurn the strategy engine behind:

- research tools
- quant dashboards
- web interfaces
- backtesting platforms
- and automated execution stacks

---

## Positioning

Octurn is best understood as:

> a strategy infrastructure layer for quantitative trading

It is **not** a broker.  
It is **not** a charting terminal.  
It is **not** just another backtester.

It is a programmable research engine that can become the core of a larger quant platform.

---

## Use Cases

Octurn can be useful for:

- **quant developers** building custom research tooling
- **systematic traders** testing new ideas quickly
- **fintech builders** embedding strategy logic into products
- **research teams** standardizing signal generation workflows
- **execution stacks** that need a clean strategy-definition layer

---

## Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

---

## Roadmap

- [ ] richer DSL syntax
- [ ] more indicators
- [ ] portfolio-level logic
- [ ] cleaner runtime summaries
- [ ] JSON export
- [ ] validation and diagnostics
- [ ] backtesting metrics
- [ ] execution bridges
- [ ] web interface integration

---

## Example Directory Structure

```text
Octurn/
├── lexer/
├── parser/
├── node/
├── interpreter/
├── ta/
├── mappers/
└── ...
```

---

## Development Philosophy

Octurn is being built with a strong emphasis on:

- **clarity of architecture**
- **strategy expressiveness**
- **runtime inspectability**
- **extensibility**
- **future execution compatibility**

The idea is to build a strong core first, then grow toward richer research and execution workflows.

---

## Project Status

Octurn is currently an early-stage infrastructure project focused on building a solid technical foundation.

The current priorities are:

- correctness
- modularity
- extensibility
- and a clear DSL/runtime contract

---

## Contact

If you are interested in:

- the engine
- collaboration
- strategy infrastructure
- execution integration
- or building on top of Octurn

feel free to open an issue or reach out.
