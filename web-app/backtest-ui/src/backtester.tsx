import { useRef } from "react";
import "./App.css";
import { show_name } from "./titles";
import { type Page } from "./pageType";
import { BacktestCard } from "./latestBackests";
import { run } from "./Octurn";

type BacktesterPageProps = {
    setPage: (page: Page) => void;
};

export function BacktesterPage({setPage}:BacktesterPageProps){
    const inputRef = useRef<HTMLTextAreaElement | null>(null);
    const handleRun = () => {
        const inputEl = inputRef.current;
        if (!inputEl) {
            return;
        }
        run(inputEl.value);
    };

    return(
    <body>
        <main className="main-backtester">
            <div className="brand-wordmark-centered">
                {show_name()}
            </div>
            <div className="hero-actions">
                <button className="primary-btn" onClick={()=> setPage("landing")}>Back</button>
                <button className="ghost-btn" onClick={()=> setPage("docs")}>View docs</button>
            </div>

            <div className="grid-container">
                <div className="terminal-outer">
                    <div className="close-expand-icons-mac">
                        <div className="mac-btn mac-btn--close"></div>
                        <div className="mac-btn mac-btn--minimize"></div>
                        <div className="mac-btn mac-btn--fullscreen"></div>
                    </div>
                    <div className="terminal-inner">
                        <div className="terminal-shell">
                            <div className="terminal-header">
                                <span className="terminal-title">Strategy console</span>
                                <span className="terminal-status">
                                    <span className="terminal-dot" aria-hidden="true"></span>
                                    ready
                                </span>
                            </div>
                            <div className="terminal-output">
                                <div className="terminal-line terminal-muted">Octurn engine online.</div>
                                <div className="terminal-line terminal-muted">Paste or type your strategy below.</div>
                            </div>
                            <div className="terminal-input-row">
                                <span className="terminal-prompt">$</span>
                                <textarea
                                    className="terminal-input"
                                    placeholder="Type strategy code..."
                                    ref={inputRef}
                                    spellCheck={false}
                                    autoCorrect="off"
                                    autoCapitalize="off"
                                    aria-label="Terminal input"
                                ></textarea>
                                <button className="terminal-send" type="button" onClick={handleRun} aria-label="Run code">
                                    Run
                                </button>
                            </div>
                        </div>
                    </div>
                </div>
                <div className="latest-backtests">
                    <BacktestCard name="Mean Reversion" winrate="72%" hash="x9qsdkjz67jdsqz"/>
                    <BacktestCard name="RSI Conversion" winrate="60%" hash="x9wzedkjz77jdsqz"/>
                    <BacktestCard name="RSI(12)>150" winrate="72%" hash="x9qsdkjz67resqz"/>
                    <BacktestCard name="MA(250)" winrate="60%" hash="x9wzedkjz77jdsq"/>
                </div>
            </div>
        </main>
    </body>
    );
}
