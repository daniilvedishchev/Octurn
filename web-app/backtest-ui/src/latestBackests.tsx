type BacktestCardProps = {
  winrate: string;
  hash: string;
  name: string;
};
  
export function BacktestCard({ winrate, hash, name }: BacktestCardProps) {
  return (
    <div className="backtest-result-element">
      <div className="metric-label" style={{gridArea: "box-name"}}>{name}</div>
      <div className="winrate-value" style={{gridArea: "box-winrate"}}>{winrate}</div>
      <div className="hash-note" style={{gridArea: "box-hash"}}>{hash}</div>
    </div>
  );
}
  