```latex
\section*{Octurn}

\textbf{Octurn} is a high‑performance C++20 core for trading strategies with a concise DSL, backtesting, and a pluggable market‑data pipeline. It is built to shorten the path from idea to verified strategy: minimal syntax, fast runtime, and a clean architecture.

\subsection*{Why Octurn}
\begin{itemize}
  \item \textbf{Expressive DSL}: describe data, parameters, indicators, and entry/exit rules in a few lines.
  \item \textbf{Speed and determinism}: C++20 core with vectorized operations over time series.
  \item \textbf{Flexible data integration}: pluggable sources with a ready Polygon API connector.
  \item \textbf{Extensible by design}: add your own indicators and operators easily.
  \item \textbf{Automation‑ready}: HTTP API (Drogon) and RabbitMQ scaffolding for queues.
  \item \textbf{WASM‑friendly}: runtime architecture prepared for WebAssembly environments.
\end{itemize}

\subsection*{What’s inside}
\begin{itemize}
  \item \texttt{lexer / parser / AST / interpreter} — complete DSL pipeline.
  \item \texttt{TA} — indicator implementations (\texttt{MA}, \texttt{RSI}).
  \item \texttt{mappers} — function registry for custom indicators.
  \item \texttt{src/polygon} — client and OHLC data loader.
  \item \texttt{my\_api} — HTTP endpoint \texttt{/backtest} and queue infrastructure.
\end{itemize}

\subsection*{DSL quick example}
\begin{verbatim}
config { equity: 100 positionSize: 1 slippageBps: 10 }
data [
  { ticker: AAPL timespan: day multiplier: 1 from: 2025-09-01 to: 2025-10-27 }
]
strategy SimpleMA {
  parameters { fast_ma: 5 slow_ma: 30 }
  indicators { RSI1 = RSI(AAPL_close, 12) }
  entry { when RSI1 > 50 }
}
\end{verbatim}

\subsection*{Build (core/CLI)}
\begin{verbatim}
cmake -S . -B build
cmake --build build
\end{verbatim}

\subsection*{Dependencies}
\begin{itemize}
  \item Core: \texttt{cpr}, \texttt{nlohmann\_json}
  \item API (optional): \texttt{Drogon}
  \item Queues (optional): \texttt{SimpleAmqpClient}
\end{itemize}

\subsection*{Status}
The core is ready for use and extension. The API and queue components are foundational scaffolding for production integrations.
```
