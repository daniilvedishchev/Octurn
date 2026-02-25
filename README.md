# Octurn

Octurn is a high‑performance C++20 core for a trading‑strategy DSL with backtesting‑oriented evaluation, a pluggable market‑data pipeline, and an interpreter runtime. It’s built to shorten the path from idea to verified strategy with minimal syntax and a clean, extensible architecture.

## DSL Example
```text
config { equity: 100 positionSize: 1 slippageBps: 10 }
data [
  { ticker: AAPL timespan: day multiplier: 1 from: 2025-09-01 to: 2025-10-27 }
]
strategy SimpleMA {
  parameters { fast_ma: 5 slow_ma: 30 }
  indicators { RSI1 = RSI(AAPL_close, 12) }
  entry { when RSI1 > 50 }
}
