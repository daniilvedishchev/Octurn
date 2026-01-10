import { WebSocketServer } from "ws";
import crypto from "crypto"


//** 
// 
// @brief open websocket on localhost:8080 
// -> clients {id:clientSocket}
// -- New connected client gets his own id and gets appended to clients, needed for the broadcast
// 
// */

const web_socket_server = new WebSocketServer({port:8080});

let clients = new Map();
let queque = new Map();

function broadcast(op){
    for (const [id,clientSocket] of clients){
        clientSocket.send(JSON.stringify({type:"SHARED_OPERATION",op}));
    }
}

function syncPeer(id,since){
    for (const[uid,clientSocket] of clients){
        clientSocket.send(JSON.stringify({type:"SYNC_PEER",for:id,from:since}));
    }
}

web_socket_server.on("connection",(clientSocket)=>{
    let id = crypto.randomUUID();
    clients.set(id,clientSocket);

    clientSocket.on("message",(raw)=>{
        let msg = JSON.parse(raw.toString());

        if (msg.type === "SHARE_OPERATION"){
            broadcast(msg.op);
        }

        if (msg.type === "SYNC_REQ"){
            let from = msg.from;
            if (!msg.from){
                from = 0;
            }
            queque.set(id,clientSocket);
            syncPeer(id,from);
        }

        if (msg.type === "SYNC_REQ_FILLED"){
            let receivingSocket = queque.get(msg.for);
            if (receivingSocket && receivingSocket.readyState === 1) {
                receivingSocket.send(JSON.stringify({ type:"SYNC_REQ_FILLED", op: msg.op }));
            }
        }
    });


})