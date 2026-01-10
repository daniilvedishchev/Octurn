import {installPolygonFetch} from "../../../src/data/polygon/polygon_fetch.js"

const apiKey = import.meta.env.VITE_POLYGON_API_KEY;

console.log("apiKey: ",apiKey);

type OcturnEngine = {
    runLexer: (script: string) => void;
    runParser: () => void;
    runInterpreter: () => void;
    isDone: () => boolean;
    tick: () => void;
    printVariables: () => void;
};
  
type OcturnModule = { Engine: new () => OcturnEngine };

let wasmModule: OcturnModule | null = null;
let engine: OcturnEngine | null = null;

async function getEngine(): Promise<OcturnEngine> {
    if (!engine) {
        wasmModule ??= (await createModule()) as OcturnModule;
        (window as any).Module = wasmModule;
        console.log("Installing polygon Fetch!");
        installPolygonFetch(wasmModule,apiKey);
        console.log("Finished installing polygon fetch");

        engine = new wasmModule.Engine();
    }
    return engine;
}

export async function run(script: string) {
    const engine = await getEngine();
    engine.runLexer(script);
    console.log("Lexer is finished;");
    engine.runParser();
    console.log("Parser is finished;");
    engine.runInterpreter();
    console.log("Interpreter is finished;");
    const loop = () => {
        try {
            console.log("entering tikc");
            engine.tick();
            console.log("tick");
            if (!engine.isDone()) {
                console.log("Not finished;");
                requestAnimationFrame(loop);
            } else {
                engine.printVariables();
            }
        } catch(error){
            console.error("WASM threw:", error);
        }
    };
    console.log("Request animation frame;");
    requestAnimationFrame(loop);
}
