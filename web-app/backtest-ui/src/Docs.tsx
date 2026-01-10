import "./App.css";
import { type Page } from "./pageType";
import { show_name } from "./titles";

type DocsPageProps = {
  setPage: (page: Page) => void;
};

type DocsSection = {
  title: string;
  subtitle: string;
  bullets: string[];
  pill?: string;
  code?: string;
  footer?: string;
};

const DOC_SECTIONS: DocsSection[] = [
  {
    title: "Connect your engine",
    subtitle: "Expose a single HTTP/WS endpoint that returns JSON.",
    bullets: [
      "Serve from localhost:4000 (configurable) with CORS enabled.",
      "Return structured data; avoid stdout scraping.",
      "Keep responses <200ms to preserve UI snappiness."
    ],
    pill: "Setup"
  },
  {
    title: "Backtest endpoint",
    subtitle: "What the UI expects when you click Run.",
    bullets: [
      "POST /backtests { strategyId, params, timeframe }",
      "Respond with id + queue status immediately.",
      "Stream progress via WebSocket channel /backtests/:id"
    ],
    pill: "API"
  },
  {
    title: "Payload shape",
    subtitle: "Minimal fields to light up the cards and charts.",
    bullets: [
      "id: string · name: string · winrate: number",
      "pnl: number[] · drawdown: number[] · logs: string[]",
      "finishedAt: ISO timestamp; status: queued|running|done"
    ],
    code: `{
  "id": "bt-42",
  "name": "Mean Reversion",
  "winrate": 0.72,
  "pnl": [120, 180, 90, 240],
  "drawdown": [0, -1.2, -0.4],
  "logs": ["boot", "fetch candles", "done"],
  "finishedAt": "2024-04-01T10:33:02Z",
  "status": "done"
}`,
    pill: "Schema"
  },
  {
    title: "Front-end hooks",
    subtitle: "Where to plug your data in the current UI.",
    bullets: [
      "latestBackests.tsx maps your results into cards.",
      "backtester.tsx: wire WebSocket messages to set state.",
      "App.css: adjust gradients or radii to match your brand."
    ],
    footer: "Tip: keep state normalized; avoid nesting large blobs in a single atom.",
    pill: "Integration"
  }
];

export function DocsPage({ setPage }: DocsPageProps) {
  return (
    <div className="docs-root">
      <div className="docs-shell">
        <div className="docs-header">
          <div>
            <div className="eyebrow">Documentation</div>
            {show_name()}
            <h2 className="docs-title">Wire your engine to the UI in minutes.</h2>
            <p className="docs-subtitle">
              Use the API contracts below to feed runs, progress, and stats into the backtester view.
              Keep payloads small and stream as you compute.
            </p>
          </div>
          <div className="docs-actions">
            <button className="ghost-btn" onClick={() => setPage("landing")}>
              Back to landing
            </button>
            <button className="primary-btn" onClick={() => setPage("backtester")}>
              Open app
            </button>
          </div>
        </div>

        <div className="docs-grid">
          {DOC_SECTIONS.map((section) => (
            <div className="docs-card" key={section.title}>
              <div className="docs-card-header">
                <div className="pill-row">
                  <span className={`pill ${section.pill === "Schema" ? "mellow" : "accent"}`}>
                    {section.pill ?? "Guide"}
                  </span>
                </div>
                <h3 className="docs-card-title">{section.title}</h3>
                <p className="docs-card-subtitle">{section.subtitle}</p>
              </div>
              <ul className="docs-list">
                {section.bullets.map((item) => (
                  <li key={item}>{item}</li>
                ))}
              </ul>
              {section.code && (
                <pre className="docs-code">
                  <code>{section.code}</code>
                </pre>
              )}
              {section.footer && <p className="docs-footer">{section.footer}</p>}
            </div>
          ))}
        </div>
      </div>
    </div>
  );
}
