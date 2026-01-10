# Octurn

Octurn is a C++20 trading-strategy DSL and backtesting core with a WebAssembly-friendly runtime and a pluggable market-data pipeline.

## Highlights
- Small, readable DSL for strategy definition (data, parameters, indicators, entry/exit)
- Built-in technical indicators (MA, RSI)
- Vectorized math and boolean logic on time series
- Async data flow for browser/WASM runtimes
- Easy extension points for new indicators and operators

## DSL at a glance
Keywords are lowercase. Data values like tickers and timespans are identifiers.

```text
data [
  { ticker: AAPL timespan: day multiplier: 1 from: 2024-01-01 to: 2024-03-01 }
]

strategy simple_ma {
  parameters { fast: 5 slow: 30 }
  indicators {
    fast_ma = MA(AAPL_close, fast)
    slow_ma = MA(AAPL_close, slow)
  }
  entry { when fast_ma > slow_ma }
}
```

## C++ usage (core)
```cpp
std::string script = R"(
  data [ { ticker: AAPL timespan: day multiplier: 1 from: 2024-01-01 to: 2024-03-01 } ]
  strategy simple_ma {
    parameters { fast: 5 slow: 30 }
    indicators { fast_ma = MA(AAPL_close, fast) }
    entry { when fast_ma > 0 }
  }
)";

Lexer lexer(script);
lexer.tokenize();

Parser parser(lexer.get_tokens());
auto root = parser.parse();

Interpreter engine(root);
engine.run();
while (!engine.is_done()) {
  engine.tick();
}
```

## Data flow
- The `data` block schedules OHLC fetches.
- When data arrives, variables are populated as:
  - `<ticker>_open`, `<ticker>_high`, `<ticker>_low`, `<ticker>_close`
- Indicators consume these vectors directly.

## Architecture
- `Octurn/lexer` - tokenization and operators
- `Octurn/parser` - AST construction
- `Octurn/node` - AST nodes and evaluation
- `Octurn/interpreter` - runtime and execution
- `Octurn/TA` - indicator implementations
- `Octurn/mappers` - function registry (indicator map)

## Extending indicators
1) Add the implementation in `Octurn/TA/taLib.cpp`
2) Register it in `Octurn/mappers/maps.cpp`

## Build notes
- C++20 is required.
- Native builds use `cpr` and `nlohmann_json`.
- WASM builds rely on a JS bridge for data fetching. You can implement your own bridge by providing:
  `request_ohlc`, `ohlc_ready`, and `get_ohlc` (see `polygon_wasm_bridge` in the full workspace).

## Status
This is the core engine. UI and JS bindings live outside this folder in the full workspace.
