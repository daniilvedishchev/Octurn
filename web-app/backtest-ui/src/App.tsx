import "./App.css";
import {useState} from "react"
import { type Page } from "./pageType";
import { LandingPage } from "./Landing";
import { BacktesterPage } from "./backtester";
import { DocsPage } from "./Docs";

function App() {
  const [page, setPage] = useState<Page>("landing");
  return (
    <main>
        {page === "landing"  && <LandingPage setPage={setPage} />}
        {page === "backtester" && <BacktesterPage setPage={setPage} />}
        {page === "docs" && <DocsPage setPage={setPage} />}
     </main>
  );
} 
export default App;
