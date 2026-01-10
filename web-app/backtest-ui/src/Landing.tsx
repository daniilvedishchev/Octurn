import "./App.css";
import { show_name } from "./titles";
import { FEATURES, FeatureCard } from "./features";
import { type Page } from "./pageType";


type LandingPageProps = {
    setPage: (page: Page) => void;
};

export function LandingPage({setPage}:LandingPageProps){
    return(
    <div className="landing-root">
        <main className="hero">
        <div className="hero-grid">
            <div className="hero-copy">
            <div className="eyebrow">Backtest cockpit</div>
            {show_name()}
            <h2 className="hero-title">
                Backtest your <span className="accent">trading ideas</span> with a modern UI.
            </h2>
            <p className="hero-subtitle">
                Connect your C++ backtesting engine to a glassy dashboard that highlights strategy
                stats, logs, and charts in one view.
            </p>
            <div className="hero-actions">
                <button className="primary-btn" onClick={()=> setPage("backtester")}>Open app</button>
                <button className="ghost-btn" onClick={()=> setPage("docs")}>View docs</button>
            </div>
            <div className="hero-metrics">
                <div className="metric-card">
                    <p className="metric-label">Engine latency</p>
                    <p className="metric-value">2.4 ms</p>
                    <p className="metric-note">per request round-trip</p>
                </div>
                <div className="metric-card">
                    <p className="metric-label">Runs queued</p>
                    <p className="metric-value">128</p>
                    <p className="metric-note">batch scenarios</p>
                </div>
                <div className="metric-card">
                    <p className="metric-label">Win-rate lift</p>
                    <p className="metric-value">+12%</p>
                    <p className="metric-note">vs last iteration</p>
                </div>
            </div>
        </div>
            <div className="hero-visual">
            <div className="visual-card">
                <div className="card-header">
                <p className="card-title">Strategy preview</p>
                <span className="pill accent">Live sync</span>
                </div>
                <p className="card-subtitle">C++ engine connected · Replay · Logs</p>
                <div className="sparkline" aria-hidden="true">
                <span className="bar bar-1"></span>
                <span className="bar bar-2"></span>
                <span className="bar bar-3"></span>
                <span className="bar bar-4"></span>
                <span className="bar bar-5"></span>
                <span className="bar bar-6"></span>
                <span className="bar bar-7"></span>
                <span className="bar bar-8"></span>
                <span className="bar bar-9"></span>
                <span className="bar bar-10"></span>
                </div>
                <div className="pill-row">
                <span className="pill accent">Latency 2.4 ms</span>
                <span className="pill">Max drawdown 3.2%</span>
                <span className="pill mellow">Sharpe 1.42</span>
                </div>
            </div>
            <div className="mini-grid">
                <div className="mini-card">
                <p className="mini-label">Execution</p>
                <p className="mini-value">Streaming</p>
                </div>
                <div className="mini-card">
                <p className="mini-label">Monte-Carlo</p>
                <p className="mini-value">512 paths</p>
                </div>
                <div className="mini-card">
                <p className="mini-label">Notebook</p>
                <p className="mini-value">Charts + Logs</p>
                </div>
            </div>
            </div>
        </div>
        <section className="features-section">
            <div className="features-header">
            <h3 className="features-title">
                Why build your own <span className="accent">strategies</span>?
            </h3>
            <p className="features-subtitle">
                Explore, validate and deploy without leaving the browser.
            </p>
            </div>
            <div className="features-grid">
            {FEATURES.map((feature) => (
                <FeatureCard
                key={feature.title}
                {...feature}
                />
            ))}
            </div>
        </section>
        </main>
    </div>);

}    
