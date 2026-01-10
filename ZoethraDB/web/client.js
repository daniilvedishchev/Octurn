let db_name = "ZOETHRADB";
let version = 1;

/**
 * @description Provides the simplest model of client side
 * Data is stored in following format: {at:Unix type,name:"operation name",loockback}
 * ---- Client:
 * -------- 1) Accepts incoming shared data
 * -------- 2) Shares data with others, when requested
 * -------- 3) Sync with other nodes
 */

function openDB(){
    return new Promise((resolve,reject)=>{
        const request = indexedDB.open(db_name,version);

        request.onupgradeneeded = (event)=>{
            event.target.result.createObjectStore("Operations",{keyPath: "at"});
        };

        request.onsuccess = (event) => {
            resolve(event.target.result);
        };
        request.onerror = (event) => reject(event.target.error?.message);
    });
}

// ============================================================ //
// Retrieve data from local storage
// ============================================================ //

function getData(db,store_name){
    return new Promise((resolve,reject)=>{
        const tx = db.transaction(store_name,"readonly");
        const os = tx.objectStore(store_name);

        const req = os.getAll();

        req.onsuccess = (event)=>{
            resolve(event.target.result);
        };
        req.onerror = (event)=>{
            reject(event.target.error);
        };
    });
}

// ============================================================ //
// Inserts new writings to local database
// ============================================================ //

function setData(db,data,store_name){
    return new Promise((resolve,reject)=>{
        const tx = db.transaction(store_name,"readwrite");
        const os = tx.objectStore(store_name);
        const req = os.put(data);

        req.onsuccess = (event)=>{
            resolve(event.target.result);
        };
        req.onerror = (event)=>{
            reject(event.target.error);
        };
    });
}

async function main(){
    // Establish client database connection
        // -> failed: write down info in console
        // -> succeed: return database object 

    let db;
    try{
        db = await openDB();
    } catch (err) {
        console.error("DB open failed", err);
    }
    
    document.getElementById("connect").addEventListener("click", () => openDB());

    const ws = new WebSocket("ws://localhost:8080");

    ws.addEventListener("open", () => {
        console.log("WebSocket connection is successfully established.");
        ws.send(JSON.stringify({type:"SYNC_REQ"}));
    });

    // ============================================================ //
    // Share with all other nodes , when new form is submited
    // ============================================================ //

    document.getElementById("create").addEventListener("click", async () => {
        const name = document.getElementById("name").value;
        const lookback = document.getElementById("lookback").value;

        const op = {at:Date.now(),name:name,lookback:lookback};
    
        await setData(db,op,"Operations");

        ws.send(JSON.stringify({type:"SHARE_OPERATION",op:op}));
    });

    ws.addEventListener("message",async(event)=>{
        const msg = JSON.parse(event.data);

        // ============================================================ //
        // Send local data if requested by participants
        // ---- based on latest available participant writing
        // ============================================================ //

        if (msg.type==="SYNC_PEER"){
            const since = msg.from;
            const history_ = await getData(db,"Operations");
            const filtered = history_.filter((item)=>item.at>since);
            ws.send(JSON.stringify({type:"SYNC_REQ_FILLED",for:msg.for,op:filtered}));
        }

        // ============================================================ //
        // If data got arrived from node -> update local state
        // ============================================================ //

        if (msg.type === "SYNC_REQ_FILLED"){
            for (const msg_op of msg.op){
                await setData(db,msg_op,"Operations");
            }
        }

        if (msg.type === "SHARED_OPERATION"){
            await setData(db,msg.op,"Operations");
        }

        // ============================================================ //
    });
}

main();
