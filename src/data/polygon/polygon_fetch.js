/** 
 @brief this file allows to fetch data with JS in async manner
        --- Parameters for API calls come from C++ Octurn engine
        --- Once fetched -> data is getting transmitted to C++ core with 'polygon_data_ready'
        --- Used memory will be deleted once operation is compeled
    
 @result C++ got fetched data + calculation result got returned by backtesting engine
*/

// ------------------------------------------------- //
//              Fetch data from polygon
// -> make it ready for C++ and send it over
// ------------------------------------------------- //
export function installPolygonFetch(Module,apiKey){

    window.fetch_polygon_data = async function (ticker,multiplier,timespan,from,to,callbackId) {
        let opens = [], highs = [], lows = [], closes = [];
        const url = "https://api.polygon.io/v2/aggs/ticker/" + ticker +
                        "/range/" + multiplier + "/" + timespan +
                        "/" + from + "/" + to + "?adjusted=true&sort=asc&limit=50000&apiKey=" + apiKey;
        
        console.log("Waiting for a response from Polygon!");

        const data = await fetch(url)
        .then((response)=>{

            if (!response.ok) {
                throw new Error(`HTTP error: ${response.status}`);
            }
            return response.json();})
            .then((data) => {

            const results = data.results ?? [];
            console.log(results);

            for (const bar of results){
                opens.push(bar["o"]);
                highs.push(bar["h"]);
                lows.push(bar["l"]);
                closes.push(bar["c"]);
            }
        }).catch((error)=> {
            console.error("Polygon fetching error!",error);
            return;
        });

        // --- Prepare data for C++ --- //
        const ohlc_data = new Module.Vec2D();
        [opens,highs,lows,closes].forEach(array => {
            let vector = new Module.VecD();
            array.forEach(element=>{vector.push_back(element)});
            ohlc_data.push_back(vector);
            vector.delete();
        });
        console.log("Polygon data ready!");
        console.log("JS lens", opens.length, highs.length, lows.length, closes.length);

        Module.polygon_data_ready(callbackId,ohlc_data);
        ohlc_data.delete();
    }
}