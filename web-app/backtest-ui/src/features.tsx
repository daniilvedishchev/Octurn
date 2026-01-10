import { useState } from "react";

type Feature = {
    title: string;
    description: string;
    details: string;
  };
  
export const FEATURES: Feature[] = [
{
    title: "Fast C++ backtesting core",
    description: "Execute thousands of simulations with your C++ engine.",
    details: "Use your own indicators and execution logic, compiled to native code for maximum speed.",
},
    {
    title: "Custom strategies",
    description: "Mix RSI, MA, MACD and custom rules.",
    details: "Define entries, exits and risk in code, then explore how tiny changes affect performance.",
},
{
    title: "Clear analytics",
    description: "See equity, drawdown, trades and win rate.",
    details: "Perfect for comparing strategies or tuning parameters over different market regimes.",
},
];
  

export function FeatureCard(props:Feature){
    const [expanded,setExpanded] = useState(false);
    return(
    <article className="feature-card"
            onClick={()=>setExpanded(!expanded)}
    >
        <div className="feature-icon" aria-hidden="true">✦</div>
        <div>
            <h3>
                {props.title}
            </h3>
            <p>{props.description}</p>
            {expanded && (<p className="feature-details">{props.details}</p>)}
        </div>
    </article>);
}
